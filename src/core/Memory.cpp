#include "core/Memory.h"

#include <spdlog/spdlog.h>

#include <iostream>
#include <algorithm>
#include <cstring>
#include <fstream>
#ifdef _WIN32
    #include <conio.h>
#else
    #include <unistd.h>
#endif

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
    if ((_currentSatp & 0x80000000) == 0) return vaddr;

    uint32_t root_ppn = _currentSatp & 0x3FFFFF;
    uint32_t root_table_addr = root_ppn * PAGE_SIZE;

    uint32_t vpn1 = (vaddr >> 22) & 0x3FF; // Top 10 bits
    uint32_t vpn0 = (vaddr >> 12) & 0x3FF; // Mid 10 bits
    uint32_t offset = vaddr & 0xFFF;       // Lower 12 bits

    uint32_t pte1_addr = root_table_addr + (vpn1 * 4);
    uint32_t pte1 = read32Physical(pte1_addr);

    if ((pte1 & 0x1) == 0) throw PageFaultException(vaddr, type);

    bool r1 = (pte1 & 0x2) != 0;
    bool w1 = (pte1 & 0x4) != 0;
    bool x1 = (pte1 & 0x8) != 0;

    uint32_t pte1_ppn = (pte1 >> 10) & 0x3FFFFF;
    uint32_t final_ppn;

    if (r1 || x1) {
        if ((!r1 && !x1) || (w1 && !r1)) {
            throw PageFaultException(vaddr, type);
        }

        if ((pte1_ppn & 0x3FF) != 0) {
            throw PageFaultException(vaddr, type);
        }

        switch (type) {
            case AccessType::InstructionFetch: if (!x1) throw PageFaultException(vaddr, type); break;
            case AccessType::Load:             if (!r1) throw PageFaultException(vaddr, type); break;
            case AccessType::Store:            if (!w1) throw PageFaultException(vaddr, type); break;
        }

        final_ppn = pte1_ppn | vpn0;
    } else {
        uint32_t leaf_table_addr = pte1_ppn * PAGE_SIZE;
        uint32_t pte0_addr = leaf_table_addr + (vpn0 * 4);
        uint32_t pte0 = read32Physical(pte0_addr);

        if ((pte0 & 0x1) == 0) throw PageFaultException(vaddr, type);

        bool r0 = (pte0 & 0x2) != 0;
        bool w0 = (pte0 & 0x4) != 0;
        bool x0 = (pte0 & 0x8) != 0;

        if ((!r0 && !x0) || (w0 && !r0)) {
            throw PageFaultException(vaddr, type);
        }

        switch (type) {
            case AccessType::InstructionFetch: if (!x0) throw PageFaultException(vaddr, type); break;
            case AccessType::Load:             if (!r0) throw PageFaultException(vaddr, type); break;
            case AccessType::Store:            if (!w0) throw PageFaultException(vaddr, type); break;
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
        return true;
    }
    if (address == 0x02004004) {
        // Scriere partea HIGH (păstrăm partea LOW intactă și înlocuim ultimii 32 biți)
        _mtimecmp = (_mtimecmp & 0x00000000FFFFFFFFULL) | (static_cast<uint64_t>(value) << 32);
        return true;
    }

    // --- UART ---
    if (address >= UART_ADDR && address < UART_ADDR + 8) {
        if (address == UART_ADDR) { // THR (Transmitter Holding Register)
            std::cout << (char)(value & 0xFF) << std::flush;
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
    if (address < 0x80000000) {
        return true;
    }

    return false;
}

bool Memory::handleMMIORead(uint32_t address, uint32_t& outValue) {
    if (address == 0x0200BFF8) { // Citește partea LOW (32 biți)
        outValue = (uint32_t)(_mtime & 0xFFFFFFFF);
        return true;
    }
    if (address == 0x0200BFFC) { // Citește partea HIGH (32 biți)
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
        if (address == UART_LSR_ADDR) {
            uint8_t lsr = 0x20; // TX Empty
            if (_uartInputChar != -1) lsr |= 0x01; // RX Data Ready
            outValue = lsr;
        }
        else if (address == UART_RHR_ADDR) {
            outValue = (_uartInputChar != -1) ? _uartInputChar : 0;
            _uartInputChar = -1;
        }
        else {
            outValue = 0; // Orice alt registru UART neimplementat returnează 0
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
        if (_uartInputChar != -1) {
            outValue = 10; // ID UART
        } else {
            outValue = 0;
        }
        return true;
    }

    if (address == 0x10001004)    { outValue = 0;    return true; }
    if (address == 0x10001000)    { outValue = _spiReadBuffer; return true; }

    if (address < 0x80000000) {
        spdlog::debug("Unmapped MMIO READ at: 0x{:08X}", address);
        outValue = 0;
        return true; // Returnăm true ca să prevenim page fault sau RAM fallback
    }

    return false;
}

void Memory::pollKeyboard() {
    if (_uartInputChar == -1) {
#ifdef _WIN32
        if (_kbhit()) {
            char c = _getch();
            if (c == '\n') c = '\r';
            _uartInputChar = c;
        }
#else
        char c;
        if (read(STDIN_FILENO, &c, 1) == 1) {
            if (c == '\n') c = '\r';
            _uartInputChar = c;
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
    uint32_t paddr = translateAddress(address, AccessType::Store);

    if (handleMMIO(paddr, value)) return;

    uint32_t pageIndex = paddr >> PAGE_SHIFT;
    uint32_t offset = paddr & PAGE_MASK;

    if (offset + sizeof(uint32_t) <= PAGE_SIZE) {
        uint8_t* ptr = getMemoryPtr(paddr, true);
        ptr[0] = value & 0xFF; ptr[1] = (value >> 8) & 0xFF;
        ptr[2] = (value >> 16) & 0xFF; ptr[3] = (value >> 24) & 0xFF;
        return;
    }
    write8(paddr, value & 0xFF); write8(paddr + 1, (value >> 8) & 0xFF);
    write8(paddr + 2, (value >> 16) & 0xFF); write8(paddr + 3, (value >> 24) & 0xFF);
}

uint32_t Memory::read32(uint32_t address, bool isInstruction) {
    AccessType type = isInstruction ? AccessType::InstructionFetch : AccessType::Load;
    uint32_t paddr = translateAddress(address, type);

    return read32Physical(paddr);
}

void Memory::write16(uint32_t address, uint16_t value) {
    uint32_t paddr = translateAddress(address, AccessType::Store);

    if (handleMMIO(paddr, value)) return;

    uint32_t pageIndex = paddr >> PAGE_SHIFT;
    uint32_t offset = paddr & PAGE_MASK;

    if (offset + sizeof(uint16_t) <= PAGE_SIZE) {
        uint8_t* ptr = getMemoryPtr(paddr, true);
        ptr[0] = value & 0xFF; ptr[1] = (value >> 8) & 0xFF;
        return;
    }
    write8(paddr, value & 0xFF); write8(paddr + 1, (value >> 8) & 0xFF);
}

uint16_t Memory::read16(uint32_t address) {
    uint32_t paddr = translateAddress(address, AccessType::Load);

    uint32_t mmioValue;
    if (handleMMIORead(paddr, mmioValue)) return mmioValue;

    uint32_t pageIndex = paddr >> PAGE_SHIFT;
    uint32_t offset = paddr & PAGE_MASK;

    if (offset + sizeof(uint16_t) <= PAGE_SIZE) {
        uint8_t* ptr = getMemoryPtr(paddr, false);
        return ptr ? (uint16_t(ptr[0]) | (uint16_t(ptr[1]) << 8)) : 0;
    }
    uint32_t b0 = read8(paddr); uint32_t b1 = read8(paddr + 1);
    return b0 | (b1 << 8);
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
