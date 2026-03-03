#include "core/Memory.h"

#include <algorithm>
#include <iostream>
#include <cstring>
#include <cassert>
#include <fstream>

Memory& Memory::getInstance() {
    static Memory instance;
    return instance;
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
    uint8_t mmioValue;
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

uint32_t Memory::translateAddress(uint32_t vaddr) {
    if ((_currentSatp & 0x80000000) == 0) return vaddr;

    uint32_t root_ppn = _currentSatp & 0x3FFFFF;
    uint32_t root_table_addr = root_ppn * PAGE_SIZE;

    uint32_t vpn1 = (vaddr >> 22) & 0x3FF; // Top 10 bits
    uint32_t vpn0 = (vaddr >> 12) & 0x3FF; // Mid 10 bits
    uint32_t offset = vaddr & 0xFFF;       // Lower 12 bits

    uint32_t pte1_addr = root_table_addr + (vpn1 * 4);
    uint32_t pte1 = read32Physical(pte1_addr);

    if ((pte1 & 0x1) == 0) return 0; // Page Fault! (Bitul V=0)
    // Here would be the Megapasses checks, but xv64 Sv32 doesn't use them by default

    uint32_t pte1_ppn = (pte1 >> 10) & 0x3FFFFF;
    uint32_t leaf_table_addr = pte1_ppn * PAGE_SIZE;

    uint32_t pte0_addr = leaf_table_addr + (vpn0 * 4);
    uint32_t pte0 = read32Physical(pte0_addr);

    if ((pte0 & 0x1) == 0) return 0; // Page Fault!

    uint32_t final_ppn = (pte0 >> 10) & 0x3FFFFF;
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
    if (address == UART_ADDR) {
        std::cout << (char)value << std::flush;
        return true;
    }

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
    return false;
}

bool Memory::handleMMIORead(uint32_t address, uint8_t& outValue) {
    if (address == UART_ADDR + 5) { outValue = 0x20; return true; }
    if (address == UART_ADDR)     { outValue = 0;    return true; }
    if (address == 0x10001004)    { outValue = 0;    return true; }
    if (address == 0x10001000)    { outValue = _spiReadBuffer; return true; }
    return false;
}

void Memory::loadDiskImage(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Error: Couldn't load fs.img!\n";
        return;
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    _disk.resize(size);
    if (file.read(reinterpret_cast<char*>(_disk.data()), size)) {
        std::cout << "[Simulator] Disk image loaded: " << size << " bytes.\n";
    }
}

void Memory::write32(uint32_t address, uint32_t value) {
    uint32_t paddr = translateAddress(address);
    if (paddr == 0) return;
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

uint32_t Memory::read32(uint32_t address) {
    uint32_t paddr = translateAddress(address);
    if (paddr == 0) return 0;
    return read32Physical(paddr);
}

void Memory::write16(uint32_t address, uint16_t value) {
    uint32_t paddr = translateAddress(address);
    if (paddr == 0) return;
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
    uint32_t paddr = translateAddress(address);
    if (paddr == 0) return 0;

    uint8_t mmioValue;
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
    uint32_t paddr = translateAddress(address);
    if (paddr == 0) return;
    if (handleMMIO(paddr, value)) return;

    uint8_t* ptr = getMemoryPtr(paddr, true);
    *ptr = value;
}

uint8_t Memory::read8(uint32_t address) {
    uint32_t paddr = translateAddress(address);
    if (paddr == 0) return 0;

    uint8_t mmioValue;
    if (handleMMIORead(paddr, mmioValue)) return mmioValue;

    uint8_t* ptr = getMemoryPtr(paddr, false);
    return ptr ? *ptr : 0;
}

void Memory::reset() {
    _pages.clear();
    _currentSatp = 0;
}
