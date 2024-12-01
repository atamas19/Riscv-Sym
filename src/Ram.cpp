#include "Ram.h"

void Ram::write32(uint32_t address, uint32_t value)
{
    memory[address]     = value & 0xFF;
    memory[address + 1] = (value >>  8) & 0xFF;
    memory[address + 2] = (value >> 16) & 0xFF;
    memory[address + 3] = (value >> 24) & 0xFF;
}

uint32_t Ram::read32(uint32_t address)
{
    return memory[address] |
            (memory[address + 1] <<  8) |
            (memory[address + 2] << 16) |
            (memory[address + 3] << 24);
}

void Ram::write16(uint32_t address, uint16_t value)
{
    memory[address]     = value & 0xFF;
    memory[address + 1] = (value >> 8) & 0xFF;
}

uint16_t Ram::read16(uint32_t address)
{
    return memory[address] |
            (memory[address + 1] << 8);
}

void Ram::write8(uint32_t address, uint8_t value)
{
    memory[address] = value & 0xFF;
}

uint8_t Ram::read8(uint32_t address)
{
    return memory[address];
}
