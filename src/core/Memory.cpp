#include "core/Memory.h"

#include <algorithm>
#include <iostream>

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

bool Memory::handleMMIO(uint32_t address, uint32_t value) {
    if (address == UART_ADDR) {
        std::cout << (char)value << std::flush;
        return true;
    }

    if (address == SYSCTRL_ADDR) {
        uint16_t code = value & 0xFFFF;

        if (code == SYSCTRL_EXIT_OK) {
            std::cout << "\n[Simulator] TEST PASSED (0x5555). Shutting down.\n";
            exit(0);
        } else if (code == SYSCTRL_EXIT_ERR) {
            std::cerr << "\n[Simulator] TEST FAILED (0x3333). Shutting down.\n";
            exit(1);
        } else {
            std::cout << "\n[Simulator] Warning: Unknown write to Test Addr: " << std::hex << value << "\n";
        }
        return true;
    }

    return false;
}

void Memory::write32(uint32_t address, uint32_t value) {
    if (handleMMIO(address, value)) return;

    write8(address,     value & 0xFF);
    write8(address + 1, (value >> 8) & 0xFF);
    write8(address + 2, (value >> 16) & 0xFF);
    write8(address + 3, (value >> 24) & 0xFF);
}

uint32_t Memory::read32(uint32_t address) {
    uint32_t b0 = read8(address);
    uint32_t b1 = read8(address + 1);
    uint32_t b2 = read8(address + 2);
    uint32_t b3 = read8(address + 3);

    return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
}

void Memory::write16(uint32_t address, uint16_t value) {
    if (handleMMIO(address, value)) return;

    write8(address,     value & 0xFF);
    write8(address + 1, (value >> 8) & 0xFF);
}

uint16_t Memory::read16(uint32_t address) {
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
    uint8_t* ptr = getMemoryPtr(address, false);

    return ptr ? *ptr : 0;
}

void Memory::reset() {
    _pages.clear();
}
