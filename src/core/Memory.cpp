#include "core/Memory.h"
#include "core/RiscvCpu.h"

#include <spdlog/spdlog.h>

#include <iostream>
#include <algorithm>
#include <cstring>
#include <fstream>
#ifdef _WIN32
    #include <conio.h>
#else
    #include <unistd.h>
    #include <sys/select.h>
#endif

namespace {
    uint32_t gWatchedRoots[8] = {};
    uint32_t gWatchedRootCount = 0;

    void rememberRootAddress(uint32_t root) {
        for (uint32_t i = 0; i < gWatchedRootCount; ++i) {
            if (gWatchedRoots[i] == root) {
                return;
            }
        }

        if (gWatchedRootCount < 8) {
            gWatchedRoots[gWatchedRootCount++] = root;
        }
    }

    bool isNearWatchedRoot(uint32_t paddr) {
        const uint64_t paddr64 = static_cast<uint64_t>(paddr);
        for (uint32_t i = 0; i < gWatchedRootCount; ++i) {
            const uint64_t root = static_cast<uint64_t>(gWatchedRoots[i]);
            if (paddr64 + 4 >= root && paddr64 < root + 0x20000ULL) {
                return true;
            }
        }
        return false;
    }
}

Memory& Memory::getInstance() {
    static Memory instance;
    return instance;
}

void Memory::incrementTime(uint64_t ticks) {
    _mtime += ticks;
}

uint8_t* Memory::getMemoryPtr(uint32_t address, bool allocateIfNeeded) {
    uint32_t pageIndex = address >> PAGE_SHIFT;
    uint32_t offset = address & PAGE_MASK;

    auto it = _pages.find(pageIndex);
    if (it != _pages.end()) {
        return &(it->second->at(offset));
    }

    if (allocateIfNeeded) {
        auto newPage = std::make_unique<Page>();
        newPage->fill(0);
        auto result = _pages.insert({pageIndex, std::move(newPage)});
        return &(result.first->second->at(offset));
    }
    return nullptr;
}

void Memory::setSATP(uint32_t satp) {
    static uint32_t satpLogCount = 0;
    if (_currentSatp != satp && satpLogCount < 64) {
        const uint32_t oldRoot = (_currentSatp & 0x3FFFFF) << 12;
        const uint32_t newRoot = (satp & 0x3FFFFF) << 12;
        const uint32_t rootEntry0 = read32Physical(newRoot);
        const uint32_t rootEntry1 = read32Physical(newRoot + 4);
        const uint32_t rootEntry2 = read32Physical(newRoot + 8);
        spdlog::info("[SATP] 0x{:08X} -> 0x{:08X} (root 0x{:08X} -> 0x{:08X})",
                     _currentSatp, satp, oldRoot, newRoot);
        spdlog::info("[SATP ROOT] root=0x{:08X} pte[0]=0x{:08X} pte[1]=0x{:08X} pte[2]=0x{:08X}",
                     newRoot, rootEntry0, rootEntry1, rootEntry2);
        ++satpLogCount;
    }
    rememberRootAddress((satp & 0x3FFFFF) << 12);
    _currentSatp = satp;
}

uint32_t Memory::read32Physical(uint32_t paddr) {
    uint32_t mmioValue;
    if (handleMMIORead(paddr, mmioValue)) return mmioValue;

    uint32_t pageIndex = paddr >> PAGE_SHIFT;
    uint32_t offset = paddr & PAGE_MASK;

    if (offset + sizeof(uint32_t) <= PAGE_SIZE) {
        auto it = _pages.find(pageIndex);
        if (it != _pages.end()) {
            uint8_t* ptr = &(it->second->at(offset));
            return uint32_t(ptr[0]) | (uint32_t(ptr[1]) << 8) |
                   (uint32_t(ptr[2]) << 16) | (uint32_t(ptr[3]) << 24);
        } else {
            return 0;
        }
    }

    uint8_t* p0 = getMemoryPtr(paddr, false);
    uint8_t* p1 = getMemoryPtr(paddr + 1, false);
    uint8_t* p2 = getMemoryPtr(paddr + 2, false);
    uint8_t* p3 = getMemoryPtr(paddr + 3, false);

    uint32_t b0 = p0 ? *p0 : 0;
    uint32_t b1 = p1 ? *p1 : 0;
    uint32_t b2 = p2 ? *p2 : 0;
    uint32_t b3 = p3 ? *p3 : 0;
    return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
}

uint32_t Memory::translateAddress(uint32_t vaddr, AccessType type) {
    PrivilegeMode effectiveMode = RiscvCpu::getInstance().getPrivilegeMode();

    // 2. Logica MPRV: Dacă suntem în M-Mode și facem Load/Store,
    // verificăm dacă OpenSBI vrea să folosească temporar regulile din S-Mode.
    if (effectiveMode == PrivilegeMode::Machine && type != AccessType::InstructionFetch) {
        uint32_t mstatus = RiscvCpu::getInstance().getCsr().read(0x300); // 0x300 este MSTATUS
        if ((mstatus & (1 << 17)) != 0) { // Bitul MPRV (Modify Privilege)
            effectiveMode = static_cast<PrivilegeMode>((mstatus >> 11) & 3); // Extragem biții MPP
        }
    }

    // 3. Dacă modul efectiv este Machine, ignorăm MMU-ul complet (returnăm adresa fizică brută)
    if (effectiveMode == PrivilegeMode::Machine) {
        return vaddr;
    }

    // 4. Dacă SATP este dezactivat (Mod Bare), ignorăm MMU-ul
    if ((_currentSatp & 0x80000000) == 0) return vaddr;

    uint32_t root_ppn = _currentSatp & 0x3FFFFF;
    uint32_t root_table_addr = root_ppn * PAGE_SIZE;

    uint32_t vpn1 = (vaddr >> 22) & 0x3FF; // Top 10 bits
    uint32_t vpn0 = (vaddr >> 12) & 0x3FF; // Mid 10 bits
    uint32_t offset = vaddr & 0xFFF;       // Lower 12 bits

    uint32_t pte1_addr = root_table_addr + (vpn1 * 4);
    uint32_t pte1 = read32Physical(pte1_addr);

    static uint32_t mmuTraceCount = 0;
    auto logMmuFault = [&](const char* reason, uint32_t pte0_addr, uint32_t pte0_value, bool hasPte0) {
        if (mmuTraceCount >= 96) {
            return;
        }
        const char* accessTypeName = "store";
        if (type == AccessType::InstructionFetch) {
            accessTypeName = "fetch";
        } else if (type == AccessType::Load) {
            accessTypeName = "load";
        }

        if (hasPte0) {
            spdlog::info(
                "[MMU TRACE] fault={} satp=0x{:08X} root=0x{:08X} vaddr=0x{:08X} access={} vpn1=0x{:03X} vpn0=0x{:03X} pte1@0x{:08X}=0x{:08X} pte0@0x{:08X}=0x{:08X}",
                reason, _currentSatp, root_table_addr, vaddr, accessTypeName, vpn1, vpn0, pte1_addr, pte1,
                pte0_addr, pte0_value);
        } else {
            spdlog::info(
                "[MMU TRACE] fault={} satp=0x{:08X} root=0x{:08X} vaddr=0x{:08X} access={} vpn1=0x{:03X} vpn0=0x{:03X} pte1@0x{:08X}=0x{:08X}",
                reason, _currentSatp, root_table_addr, vaddr, accessTypeName, vpn1, vpn0, pte1_addr, pte1);
        }
        ++mmuTraceCount;
    };

    if ((pte1 & 0x1) == 0) {
        logMmuFault("invalid-l1-v", 0, 0, false);
        throw PageFaultException(vaddr, type);
    }

    bool r1 = (pte1 & 0x2) != 0;
    bool w1 = (pte1 & 0x4) != 0;
    bool x1 = (pte1 & 0x8) != 0;

    uint32_t pte1_ppn = (pte1 >> 10) & 0x3FFFFF;
    uint32_t final_ppn;

    if (r1 || x1) {
        if ((!r1 && !x1) || (w1 && !r1)) {
            logMmuFault("invalid-l1-rwx", 0, 0, false);
            throw PageFaultException(vaddr, type);
        }

        if ((pte1_ppn & 0x3FF) != 0) {
            logMmuFault("misaligned-superpage", 0, 0, false);
            throw PageFaultException(vaddr, type);
        }

        switch (type) {
            case AccessType::InstructionFetch:
                if (!x1) {
                    logMmuFault("perm-x-l1", 0, 0, false);
                    throw PageFaultException(vaddr, type);
                }
                break;
            case AccessType::Load:
                if (!r1) {
                    logMmuFault("perm-r-l1", 0, 0, false);
                    throw PageFaultException(vaddr, type);
                }
                break;
            case AccessType::Store:
                if (!w1) {
                    logMmuFault("perm-w-l1", 0, 0, false);
                    throw PageFaultException(vaddr, type);
                }
                break;
        }

        // --- NEW PTE1 U/A/D CHECKS ---
        bool u1 = (pte1 & 0x10) != 0;
        bool a1 = (pte1 & 0x40) != 0;
        bool d1 = (pte1 & 0x80) != 0;

        if (effectiveMode == PrivilegeMode::User && !u1) {
            logMmuFault("perm-u-l1", 0, 0, false);
            throw PageFaultException(vaddr, type);
        }
        if (effectiveMode == PrivilegeMode::Supervisor && u1) {
            if (type == AccessType::InstructionFetch) {
                logMmuFault("perm-s-exec-u-l1", 0, 0, false);
                throw PageFaultException(vaddr, type);
            }
            uint32_t sstatus = RiscvCpu::getInstance().getCsr().read(0x100);
            if ((sstatus & (1 << 18)) == 0) { // Check SUM bit
                logMmuFault("perm-s-sum-u-l1", 0, 0, false);
                throw PageFaultException(vaddr, type);
            }
        }
        if (!a1 || (type == AccessType::Store && !d1)) {
            logMmuFault("ad-bits-missing-l1", 0, 0, false);
            throw PageFaultException(vaddr, type);
        }

        final_ppn = pte1_ppn | vpn0;
    } else {
        uint32_t leaf_table_addr = pte1_ppn * PAGE_SIZE;
        uint32_t pte0_addr = leaf_table_addr + (vpn0 * 4);
        uint32_t pte0 = read32Physical(pte0_addr);

        if ((pte0 & 0x1) == 0) {
            logMmuFault("invalid-l0-v", pte0_addr, pte0, true);
            throw PageFaultException(vaddr, type);
        }

        bool r0 = (pte0 & 0x2) != 0;
        bool w0 = (pte0 & 0x4) != 0;
        bool x0 = (pte0 & 0x8) != 0;

        if ((!r0 && !x0) || (w0 && !r0)) {
            logMmuFault("invalid-l0-rwx", pte0_addr, pte0, true);
            throw PageFaultException(vaddr, type);
        }

        switch (type) {
            case AccessType::InstructionFetch:
                if (!x0) {
                    logMmuFault("perm-x-l0", pte0_addr, pte0, true);
                    throw PageFaultException(vaddr, type);
                }
                break;
            case AccessType::Load:
                if (!r0) {
                    logMmuFault("perm-r-l0", pte0_addr, pte0, true);
                    throw PageFaultException(vaddr, type);
                }
                break;
            case AccessType::Store:
                if (!w0) {
                    logMmuFault("perm-w-l0", pte0_addr, pte0, true);
                    throw PageFaultException(vaddr, type);
                }
                break;
        }

        // --- NEW PTE0 U/A/D CHECKS ---
        bool u0 = (pte0 & 0x10) != 0;
        bool a0 = (pte0 & 0x40) != 0;
        bool d0 = (pte0 & 0x80) != 0;

        if (effectiveMode == PrivilegeMode::User && !u0) {
            logMmuFault("perm-u-l0", pte0_addr, pte0, true);
            throw PageFaultException(vaddr, type);
        }
        if (effectiveMode == PrivilegeMode::Supervisor && u0) {
            if (type == AccessType::InstructionFetch) {
                logMmuFault("perm-s-exec-u-l0", pte0_addr, pte0, true);
                throw PageFaultException(vaddr, type);
            }
            uint32_t sstatus = RiscvCpu::getInstance().getCsr().read(0x100);
            if ((sstatus & (1 << 18)) == 0) { // Check SUM bit
                logMmuFault("perm-s-sum-u-l0", pte0_addr, pte0, true);
                throw PageFaultException(vaddr, type);
            }
        }
        if (!a0 || (type == AccessType::Store && !d0)) {
            logMmuFault("ad-bits-missing-l0", pte0_addr, pte0, true);
            throw PageFaultException(vaddr, type);
        }

        final_ppn = (pte0 >> 10) & 0x3FFFFF;
    }

    uint32_t physical_address = (final_ppn * PAGE_SIZE) + offset;
    return physical_address;
}

static inline uint16_t getCRC16(const uint8_t* message, int length) {
    uint32_t crc = 0x0000;
    for (int i = 0; i < length; i++) {
        crc ^= (message[i] << 8);
        for (int j = 0; j < 8; j++) {
            crc <<= 1;
            if (crc & (1 << 16)) {
                crc ^= 0x11021;
            }
        }
    }
    return crc & 0xFFFF;
}

bool Memory::handleMMIO(uint32_t address, uint32_t value) {
    // --- CLINT: mtimecmp ---
    if (address == 0x02004000) {
        // Scriere partea LOW (păstrăm partea HIGH intactă și înlocuim primii 32 biți)
        _mtimecmp = (_mtimecmp & 0xFFFFFFFF00000000ULL) | value;
        spdlog::info("CLINT: write mtimecmp_low = 0x{:08X}, new mtimecmp=0x{:016X}", value, _mtimecmp);
        return true;
    }
    if (address == 0x02004004) {
        // Scriere partea HIGH (păstrăm partea LOW intactă și înlocuim ultimii 32 biți)
        _mtimecmp = (_mtimecmp & 0x00000000FFFFFFFFULL) | (static_cast<uint64_t>(value) << 32);
        spdlog::info("CLINT: write mtimecmp_high = 0x{:08X}, new mtimecmp=0x{:016X}", value, _mtimecmp);
        return true;
    }

    // --- UART ---
    if (address >= UART_ADDR && address < UART_ADDR + 8) {
        uint32_t offset = address - UART_ADDR;

        if (offset == 0) {
            if ((_uartRegs[3] & 0x80) == 0) { // DLAB = 0
                std::cout << (char)(value & 0xFF) << std::flush;
                _uartTxIrq = true; // Bufferul e gol, declanșăm întrerupere TX!
            } else { // DLAB = 1
                _uartRegs[0] = value & 0xFF;
            }
        } else if (offset == 1) { // IER
            _uartRegs[1] = value & 0xFF;
            // Dacă Linux tocmai a activat TX Interrupts, îi semnalăm că bufferul e deja gol
            if (value & 0x02) _uartTxIrq = true;
        } else {
            _uartRegs[offset] = value & 0xFF;
        }
        return true;
    }

    if (address == PLIC_SCLAIM_ADDR) return true; // PLIC COMPLETE

    if (address == 0x10001004) return true; // SDCARD CTR
    if (address == 0x10001000) {            // SDCARD RW
        uint8_t byteVal = value & 0xFF;

        if (_spiState == 0) {
            _spiReadBuffer = 0xFF;
            if (byteVal != 0xFF) {
                _spiCmd = byteVal & 0x3F;
                _spiState = 1;
                _spiArg = 0;
                _spiArgBytesReceived = 0;
            }
        }
        else if (_spiState == 1) {
            _spiArg = (_spiArg << 8) | byteVal;
            _spiArgBytesReceived++;
            if (_spiArgBytesReceived == 4) _spiState = 2;
        }
        else if (_spiState == 2) {
            _spiReadBuffer = 0x00;
            if (_spiCmd == 17) {
                _spiState = 3;
                _spiDataBytesTransferred = 0;
            } else if (_spiCmd == 24) {
                _spiState = 4;
                _spiDataBytesTransferred = 0;
            } else {
                _spiState = 0;
            }
        }
        else if (_spiState == 3) {
            if (_spiDataBytesTransferred == 0) {
                _spiReadBuffer = 0x00;
                uint32_t diskOffset = _spiArg * 512;
                _spiCurrentCrc = (diskOffset + 512 <= _disk.size()) ? getCRC16(&_disk[diskOffset], 512) : 0;
                _spiDataBytesTransferred++;
            } else if (_spiDataBytesTransferred == 1) {
                _spiReadBuffer = 0xFE;
                _spiDataBytesTransferred++;
            } else if (_spiDataBytesTransferred <= 513) {
                uint32_t diskOffset = (_spiArg * 512) + (_spiDataBytesTransferred - 2);
                _spiReadBuffer = (diskOffset < _disk.size()) ? _disk[diskOffset] : 0;
                _spiDataBytesTransferred++;
            } else if (_spiDataBytesTransferred == 514) {
                _spiReadBuffer = (_spiCurrentCrc >> 8) & 0xFF;
                _spiDataBytesTransferred++;
            } else if (_spiDataBytesTransferred == 515) {
                _spiReadBuffer = _spiCurrentCrc & 0xFF;
                _spiDataBytesTransferred++;
            } else {
                _spiReadBuffer = 0xFF;
                _spiState = 0;
            }
        }
        else if (_spiState == 4) {
            if (_spiDataBytesTransferred == 0) {
                static int clock_count = 0;
                if (clock_count == 0) { _spiReadBuffer = 0x00; clock_count++; }
                else { _spiReadBuffer = 0xFF; }

                if (byteVal == 0xFE) {
                    _spiDataBytesTransferred = 1;
                    clock_count = 0;
                }
            } else if (_spiDataBytesTransferred <= 512) {
                uint32_t diskOffset = (_spiArg * 512) + (_spiDataBytesTransferred - 1);
                if (diskOffset < _disk.size()) _disk[diskOffset] = byteVal;
                _spiDataBytesTransferred++;
            } else if (_spiDataBytesTransferred <= 514) {
                _spiDataBytesTransferred++;
            } else {
                _spiReadBuffer = 0x05;
                _spiState = 0;
            }
        }
        return true;
    }

    // Fallback de siguranță: dacă se face o scriere sub adresa de bază a RAM-ului (0x80000000)
    // la care nu ai implementat încă hardware-ul, ignorăm scrierea în loc să alocăm memorie.
    // if (address < 0x80000000) {
    //     return true;
    // }

    return false;
}

bool Memory::handleMMIORead(uint32_t address, uint32_t& outValue) {
    if (address == 0x0200BFF8) {
        outValue = (uint32_t)(_mtime & 0xFFFFFFFF);
        return true;
    }
    if (address == 0x0200BFFC) {
        outValue = (uint32_t)((_mtime >> 32) & 0xFFFFFFFF);
        return true;
    }
    if (address == 0x02004000) {
        outValue = static_cast<uint32_t>(_mtimecmp & 0xFFFFFFFF);
        return true;
    }
    if (address == 0x02004004) {
        outValue = static_cast<uint32_t>((_mtimecmp >> 32) & 0xFFFFFFFF);
        return true;
    }

    // --- UART ---
    if (address >= UART_ADDR && address < UART_ADDR + 8) {
        uint32_t offset = address - UART_ADDR;

        if (offset == 0) {
            if ((_uartRegs[3] & 0x80) != 0) { // DLAB = 1
                outValue = _uartRegs[0];
            } else { // DLAB = 0 (RHR - Citim tasta)
                outValue = (_uartInputChar != -1) ? _uartInputChar : 0;
                _uartInputChar = -1; // Tasta a fost consumată
            }
        } else if (offset == 1) {
            outValue = _uartRegs[1]; // IER / DLM
        } else if (offset == 2) {
            // IIR: Stabilim ce i-a cauzat lui Linux întreruperea
            if ((_uartInputChar != -1) && (_uartRegs[1] & 0x01)) {
                outValue = 0xC4; // RX Data Available (Tastă apăsată)
            } else if (_uartTxIrq && (_uartRegs[1] & 0x02)) {
                outValue = 0xC2; // TX Holding Register Empty (Sunt gata de printat următoarea literă)
                _uartTxIrq = false; // Hardware-ul curăță flag-ul când IIR este citit!
            } else {
                outValue = 0xC1; // No Interrupt Pending
            }
        } else if (offset == 3) {
            outValue = _uartRegs[3]; // LCR
        } else if (offset == 4) {
            outValue = _uartRegs[4]; // MCR
        } else if (offset == 5) {
            uint8_t lsr = 0x60;
            if (_uartInputChar != -1) lsr |= 0x01;
            outValue = lsr;
        } else if (offset == 6) {
            outValue = 0x00; // MSR
        } else if (offset == 7) {
            outValue = _uartRegs[7]; // SPR
        } else {
            outValue = 0;
        }
        return true;
    }

    // --- UART LSR ---
    if (address == UART_LSR_ADDR) {
        uint8_t lsr = 0x20; // TX Empty
        if (_uartInputChar != -1) {
            lsr |= 0x01; // RX Data Ready
        }
        outValue = lsr;
        return true;
    }

    // --- UART RHR ---
    if (address == UART_RHR_ADDR) {
        if (_uartInputChar != -1) {
            outValue = _uartInputChar;
            _uartInputChar = -1;
        } else {
            outValue = 0;
        }
        return true;
    }

    // --- PLIC SCLAIM ---
    if (address == PLIC_SCLAIM_ADDR) {
        if (isUartIrqPending()) {
            outValue = 12;
        } else {
            outValue = 0;
        }
        return true;
    }

    if (address == 0x10001004)    { outValue = 0;    return true; }
    if (address == 0x10001000)    { outValue = _spiReadBuffer; return true; }

    // if (address < 0x80000000) {
    //     spdlog::debug("Unmapped MMIO READ at: 0x{:08X}", address);
    //     outValue = 0;
    //     return true; // Returnăm true ca să prevenim page fault sau RAM fallback
    // }

    return false;
}

void Memory::pollKeyboard() {
    if (_uartInputChar == -1) {
#ifdef _WIN32
        if (_kbhit()) {
            char c = _getch();
            if (c == '\n') c = '\r';
            _uartInputChar = c;
            _uartIrqPending = true;
        }
#else
        struct timeval tv = { 0L, 0L };
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);

        if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0) {
            char c;
            if (read(STDIN_FILENO, &c, 1) == 1) {
                if (c == '\r' || c == '\n') {
                    c = '\n';
                }

                if (c == '\n') spdlog::critical("Received char from keyboard: [ENTER]");
                else spdlog::critical("Received char from keyboard: {}", c);

                _uartInputChar = c;
                _uartIrqPending = true;
            }
        }
#endif
    }
}

bool Memory::loadDiskImage(const std::string& path) {
    if (path.empty()) {
        return true;
    }
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        spdlog::error("Couldn't load image from {}", path);
        return false;
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    _disk.resize(size);
    if (file.read(reinterpret_cast<char*>(_disk.data()), size)) {
        spdlog::info("Disk image loaded: {} bytes", size);
        return true;
    }
    return false;
}

void Memory::write32(uint32_t address, uint32_t value) {
    uint32_t offset = address & PAGE_MASK;
    if (offset + sizeof(uint32_t) <= PAGE_SIZE) {
        uint32_t paddr = translateAddress(address, AccessType::Store);
        if (handleMMIO(paddr, value)) return;
        uint8_t* ptr = getMemoryPtr(paddr, true);
        ptr[0] = value & 0xFF; ptr[1] = (value >> 8) & 0xFF;
        ptr[2] = (value >> 16) & 0xFF; ptr[3] = (value >> 24) & 0xFF;
    } else {
        write8(address, value & 0xFF);
        write8(address + 1, (value >> 8) & 0xFF);
        write8(address + 2, (value >> 16) & 0xFF);
        write8(address + 3, (value >> 24) & 0xFF);
    }
}

uint32_t Memory::read32(uint32_t address, bool isInstruction) {
    AccessType type = isInstruction ? AccessType::InstructionFetch : AccessType::Load;
    uint32_t offset = address & PAGE_MASK;

    if (offset + sizeof(uint32_t) <= PAGE_SIZE) {
        uint32_t paddr = translateAddress(address, type);
        return read32Physical(paddr);
    } else {
        uint32_t b0 = read8(address);
        uint32_t b1 = read8(address + 1);
        uint32_t b2 = read8(address + 2);
        uint32_t b3 = read8(address + 3);
        return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
    }
}

void Memory::write16(uint32_t address, uint16_t value) {
    uint32_t offset = address & PAGE_MASK;
    if (offset + sizeof(uint16_t) <= PAGE_SIZE) {
        uint32_t paddr = translateAddress(address, AccessType::Store);
        if (handleMMIO(paddr, value)) return;
        uint8_t* ptr = getMemoryPtr(paddr, true);
        ptr[0] = value & 0xFF; ptr[1] = (value >> 8) & 0xFF;
    } else {
        write8(address, value & 0xFF);
        write8(address + 1, (value >> 8) & 0xFF);
    }
}

uint16_t Memory::read16(uint32_t address) {
    uint32_t offset = address & PAGE_MASK;
    if (offset + sizeof(uint16_t) <= PAGE_SIZE) {
        uint32_t paddr = translateAddress(address, AccessType::Load);
        uint32_t mmioValue;
        if (handleMMIORead(paddr, mmioValue)) return mmioValue;
        uint8_t* ptr = getMemoryPtr(paddr, false);
        return ptr ? (uint16_t(ptr[0]) | (uint16_t(ptr[1]) << 8)) : 0;
    } else {
        uint32_t b0 = read8(address);
        uint32_t b1 = read8(address + 1);
        return b0 | (b1 << 8);
    }
}

void Memory::write8(uint32_t address, uint8_t value) {
    uint32_t paddr = translateAddress(address, AccessType::Store);

    if (handleMMIO(paddr, value)) return;

    uint8_t* ptr = getMemoryPtr(paddr, true);
    *ptr = value;
}

uint8_t Memory::read8(uint32_t address) {
    uint32_t paddr = translateAddress(address, AccessType::Load);


    uint32_t mmioValue;
    if (handleMMIORead(paddr, mmioValue)) return mmioValue;

    uint8_t* ptr = getMemoryPtr(paddr, false);
    return ptr ? *ptr : 0;
}

void Memory::reset() {
    _pages.clear();
    _currentSatp = 0;
}
