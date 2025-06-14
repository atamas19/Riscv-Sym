#include "Memory.h"

#include <algorithm>

Memory& Memory::getInstance()
{
    static Memory instance;

    return instance;
}

void Memory::write32(uint32_t address, int32_t value)
{
    memory.at(address)     = value & 0xFF;
    memory.at(address + 1) = (value >>  8) & 0xFF;
    memory.at(address + 2) = (value >> 16) & 0xFF;
    memory.at(address + 3) = (value >> 24) & 0xFF;
}

int32_t Memory::read32(uint32_t address)
{
    return memory.at(address) |
            (memory.at(address + 1) <<  8) |
            (memory.at(address + 2) << 16) |
            (memory.at(address + 3) << 24);
}

void Memory::write16(uint32_t address, int16_t value)
{
    memory.at(address)     = value & 0xFF;
    memory.at(address + 1) = (value >> 8) & 0xFF;
}

int16_t Memory::read16(uint32_t address)
{
    return memory.at(address) |
            (memory.at(address + 1) << 8);
}

void Memory::write8(uint32_t address, int8_t value)
{
    memory.at(address) = value & 0xFF;
}

int8_t Memory::read8(uint32_t address)
{
    return memory.at(address);
}

void Memory::reset()
{
    std::fill(memory.begin(), memory.end(), 0);
}
