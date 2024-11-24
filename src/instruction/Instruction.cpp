#include "instruction/Instruction.h"

#include <stdexcept>

uint32_t getBits(uint32_t instruction, uint8_t x, uint8_t y) 
{
    if (x > y || y >= 32)
        throw std::out_of_range("Invalid bit range");

    uint32_t mask = (1 << (y - x + 1)) - 1;
    return (instruction >> x) & mask;
}