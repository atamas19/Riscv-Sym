#include "Memory.h"

#include <algorithm>

Memory& Memory::getInstance()
{
    static Memory instance;

    return instance;
}

void Memory::write32(uint32_t address, int32_t value)
{
    memory[address]     = value & 0xFF;
    memory[address + 1] = (value >>  8) & 0xFF;
    memory[address + 2] = (value >> 16) & 0xFF;
    memory[address + 3] = (value >> 24) & 0xFF;
}

int32_t Memory::read32(uint32_t address)
{
    return memory[address] |
            (memory[address + 1] <<  8) |
            (memory[address + 2] << 16) |
            (memory[address + 3] << 24);
}

void Memory::write16(uint32_t address, int16_t value)
{
    memory[address]     = value & 0xFF;
    memory[address + 1] = (value >> 8) & 0xFF;
}

int16_t Memory::read16(uint32_t address)
{
    return memory[address] |
            (memory[address + 1] << 8);
}

void Memory::write8(uint32_t address, int8_t value)
{
    memory[address] = value & 0xFF;
}

int8_t Memory::read8(uint32_t address)
{
    return memory[address];
}

void Memory::reset()
{
    std::fill(memory.begin(), memory.end(), 0);
}
