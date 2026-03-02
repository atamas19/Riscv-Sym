#include "core/Memory.h"

#include <algorithm>
#include <iostream>
#include <cstring>
#include <cassert>
#include <fstream>

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

Memory& Memory::getInstance() {
    static Memory instance;

    return instance;
}

static uint16_t getCRC16(const uint8_t* message, int length) {
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
    // --- UART ---
    if (address == 0x10000000) {
        std::cout << (char)value << std::flush;
        return true;
    }

    // --- SDCARD CTR (Control Register) ---
    if (address == 0x10001004) {
        // Ignorăm semnalele de control hardware (CS, SCK) în emulator
        return true; 
    }

    // --- SDCARD RW (Write) ---
    if (address == 0x10001000) {
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
            if (_spiArgBytesReceived == 4) {
                _spiState = 2;
            }
        }
        else if (_spiState == 2) {
            // Am primit CRC. La următorul ceas, răspundem cu R1 (Success = 0x00)
            _spiReadBuffer = 0x00; 
            if (_spiCmd == 17) {
                _spiState = 3; // Mod citire bloc
                _spiDataBytesTransferred = 0;
            } else if (_spiCmd == 24) {
                _spiState = 4; // Mod scriere bloc
                _spiDataBytesTransferred = 0;
            } else {
                _spiState = 0;
            }
        }
        else if (_spiState == 3) { // READ BLOCK (Trimitem spre kernel)
            if (_spiDataBytesTransferred == 0) {
                _spiReadBuffer = 0x00; // Menținem R1 
                
                // NOU: Calculăm CRC-ul pentru cei 512 octeți pe care urmează să-i trimitem
                uint32_t diskOffset = _spiArg * 512;
                if (diskOffset + 512 <= _disk.size()) {
                    _spiCurrentCrc = getCRC16(&_disk[diskOffset], 512);
                } else {
                    _spiCurrentCrc = 0;
                }
                
                _spiDataBytesTransferred++;
            } else if (_spiDataBytesTransferred == 1) {
                _spiReadBuffer = 0xFE; // Data Token
                _spiDataBytesTransferred++;
            } else if (_spiDataBytesTransferred <= 513) { // Cei 512 octeți de date
                uint32_t diskOffset = (_spiArg * 512) + (_spiDataBytesTransferred - 2);
                _spiReadBuffer = (diskOffset < _disk.size()) ? _disk[diskOffset] : 0;
                _spiDataBytesTransferred++;
            } else if (_spiDataBytesTransferred == 514) {
                // NOU: Trimitem CRC High Byte
                _spiReadBuffer = (_spiCurrentCrc >> 8) & 0xFF; 
                _spiDataBytesTransferred++;
            } else if (_spiDataBytesTransferred == 515) {
                // NOU: Trimitem CRC Low Byte
                _spiReadBuffer = _spiCurrentCrc & 0xFF;
                _spiDataBytesTransferred++;
            } else {
                _spiReadBuffer = 0xFF;
                _spiState = 0; // Gata transferul
            }
        }
        else if (_spiState == 4) { // WRITE BLOCK (Primim de la kernel)
            if (_spiDataBytesTransferred == 0) {
                static int clock_count = 0;
                if (clock_count == 0) { _spiReadBuffer = 0x00; clock_count++; } 
                else { _spiReadBuffer = 0xFF; }
                
                if (byteVal == 0xFE) { // Am primit Data Token
                    _spiDataBytesTransferred = 1;
                    clock_count = 0;
                }
            } else if (_spiDataBytesTransferred <= 512) {
                uint32_t diskOffset = (_spiArg * 512) + (_spiDataBytesTransferred - 1);
                if (diskOffset < _disk.size()) {
                    _disk[diskOffset] = byteVal;
                }
                _spiDataBytesTransferred++;
            } else if (_spiDataBytesTransferred <= 514) {
                _spiDataBytesTransferred++; // Ignorăm CRC primit
            } else {
                _spiReadBuffer = 0x05; // Acceptat
                _spiState = 0;
            }
        }
        return true;
    }
    return false;
}

bool Memory::handleMMIORead(uint32_t address, uint8_t& outValue) {
    // --- UART LSR ---
    if (address == 0x10000005) {
        outValue = 0x20;
        return true;
    }
    // --- UART RHR (Tastatura - momentan blocata) ---
    if (address == 0x10000000) {
        outValue = 0; 
        return true;
    }

    // --- SDCARD CTR ---
    if (address == 0x10001004) {
        outValue = 0;
        return true;
    }

    // --- SDCARD RW (Read) ---
    if (address == 0x10001000) {
        // Returnăm doar buffer-ul. NU mai avansăm starea aici!
        outValue = _spiReadBuffer; 
        return true;
    }
    // --- SDCARD CTR ---
    if (address == 0x10001004) {
        outValue = 0;
        return true;
    }
    return false;
}

void Memory::loadDiskImage(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Eroare: Nu am putut deschide fs.img!\n";
        return;
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    _disk.resize(size);
    if (file.read(reinterpret_cast<char*>(_disk.data()), size)) {
        std::cout << "[Simulator] Disk image încărcată: " << size << " bytes.\n";
    }
}

void Memory::write32(uint32_t address, uint32_t value) {
    if (handleMMIO(address, value)) return;

    uint32_t pageIndex = address >> PAGE_SHIFT;
    uint32_t offset = address & PAGE_MASK;

    if (offset + sizeof(uint32_t) <= PAGE_SIZE) {
        uint8_t* ptr = nullptr;

        auto it = _pages.find(pageIndex);
        if (it != _pages.end()) {
            ptr = &(it->second->at(offset));
        } else {
            ptr = getMemoryPtr(address, true);
        }
        assert(ptr != nullptr);

        ptr[0] = value & 0xFF;
        ptr[1] = (value >> 8) & 0xFF;
        ptr[2] = (value >> 16) & 0xFF;
        ptr[3] = (value >> 24) & 0xFF;
        return;
    }

    write8(address,     value & 0xFF);
    write8(address + 1, (value >> 8) & 0xFF);
    write8(address + 2, (value >> 16) & 0xFF);
    write8(address + 3, (value >> 24) & 0xFF);
}

uint32_t Memory::read32(uint32_t address) {
    uint8_t mmioValue;
    if (handleMMIORead(address, mmioValue)) return mmioValue;

    uint32_t pageIndex = address >> PAGE_SHIFT;
    uint32_t offset = address & PAGE_MASK;

    if (offset + sizeof(uint32_t) <= PAGE_SIZE) {
        auto it = _pages.find(pageIndex);
        if (it != _pages.end()) {
            uint8_t* ptr = &(it->second->at(offset));
            assert(ptr != nullptr);

            return uint32_t(ptr[0]) |
                   (uint32_t(ptr[1]) << 8) |
                   (uint32_t(ptr[2]) << 16) |
                   (uint32_t(ptr[3]) << 24);
        } else {
            return 0;
        }
    }

    uint32_t b0 = read8(address);
    uint32_t b1 = read8(address + 1);
    uint32_t b2 = read8(address + 2);
    uint32_t b3 = read8(address + 3);

    return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
}

void Memory::write16(uint32_t address, uint16_t value) {
    if (handleMMIO(address, value)) return;

    uint32_t pageIndex = address >> PAGE_SHIFT;
    uint32_t offset = address & PAGE_MASK;

    if (offset + sizeof(uint16_t) <= PAGE_SIZE) {
        uint8_t* ptr = nullptr;

        auto it = _pages.find(pageIndex);
        if (it != _pages.end()) {
            ptr = &(it->second->at(offset));
        } else {
            ptr = getMemoryPtr(address, true);
        }
        assert(ptr != nullptr);

        ptr[0] = value & 0xFF;
        ptr[1] = (value >> 8) & 0xFF;
        return;
    }

    write8(address,     value & 0xFF);
    write8(address + 1, (value >> 8) & 0xFF);
}

uint16_t Memory::read16(uint32_t address) {
    uint8_t mmioValue;
    if (handleMMIORead(address, mmioValue)) return mmioValue;

    uint32_t pageIndex = address >> PAGE_SHIFT;
    uint32_t offset = address & PAGE_MASK;

    if (offset + sizeof(uint16_t) <= PAGE_SIZE) {
        auto it = _pages.find(pageIndex);
        if (it != _pages.end()) {
            uint8_t* ptr = &(it->second->at(offset));
            assert(ptr != nullptr);

            return uint16_t(ptr[0]) | (uint16_t(ptr[1]) << 8);
        } else {
            return 0;
        }
    }

    uint32_t b0 = read8(address);
    uint32_t b1 = read8(address + 1);

    return b0 | (b1 << 8);
}

void Memory::write8(uint32_t address, uint8_t value) {
    if (handleMMIO(address, value)) return;

    uint8_t* ptr = getMemoryPtr(address, true);
    *ptr = value;
}

uint8_t Memory::read8(uint32_t address) {
    uint8_t mmioValue;
    if (handleMMIORead(address, mmioValue)) {
        return mmioValue; // Bypass la RAM!
    }

    uint8_t* ptr = getMemoryPtr(address, false);

    return ptr ? *ptr : 0;
}

void Memory::reset() {
    _pages.clear();
}
