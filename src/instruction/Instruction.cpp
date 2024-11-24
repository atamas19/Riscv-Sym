#include "instruction/Instruction.h"

uint32_t getBits(uint32_t instruction, uint8_t x, uint8_t y)
{
    uint32_t mask = ((1 << (y - x + 1)) - 1) << x;
    
    return (instruction & mask) >> x;
}