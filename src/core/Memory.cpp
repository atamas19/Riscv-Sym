#include "core/Memory.h"

#include <algorithm>
#include <iostream>

#define UART_ADDR           0x10000000
#define SYSCTRL_ADDR        0x10000020
#define SYSCTRL_EXIT_OK     0x5555
#define SYSCTRL_EXIT_ERR    0x3333

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

    _memory.at(address)     = value & 0xFF;
    _memory.at(address + 1) = (value >>  8) & 0xFF;
    _memory.at(address + 2) = (value >> 16) & 0xFF;
    _memory.at(address + 3) = (value >> 24) & 0xFF;
}

uint32_t Memory::read32(uint32_t address) {
    return _memory.at(address) |
            (_memory.at(address + 1) <<  8) |
            (_memory.at(address + 2) << 16) |
            (_memory.at(address + 3) << 24);
}

void Memory::write16(uint32_t address, uint16_t value) {
    if (handleMMIO(address, value)) return;

    _memory.at(address)     = value & 0xFF;
    _memory.at(address + 1) = (value >> 8) & 0xFF;
}

uint16_t Memory::read16(uint32_t address) {
    return _memory.at(address) |
            (_memory.at(address + 1) << 8);
}

void Memory::write8(uint32_t address, uint8_t value) {
    if (handleMMIO(address, value)) return;

    _memory.at(address) = value & 0xFF;
}

uint8_t Memory::read8(uint32_t address) {
    return _memory.at(address);
}

void Memory::reset() {
    std::fill(_memory.begin(), _memory.end(), 0);
}
