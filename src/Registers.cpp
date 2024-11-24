#include "Registers.h"

#include <stdexcept>

uint32_t Registers::getRegister(uint8_t registerId)
{
    if (registerId >= 0 && registerId < 32)
        return regs[registerId];
    else
        throw std::out_of_range("Register id out of bounds");
}

void Registers::setRegister(uint8_t registerId, uint32_t registerValue)
{
    if (registerId > 0 && registerId < 32)
        regs[registerId] = registerValue;
    else
        throw std::out_of_range("Register id out of bounds");
}
