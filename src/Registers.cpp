#include "Registers.h"

#include <stdexcept>

uint8_t Registers::checkRegisterId(uint8_t registerId)
{
    if (registerId > 0 && registerId < 32)
        return registerId;
    else
        throw std::out_of_range("Register id out of bounds");
}

uint32_t Registers::getRegister(uint8_t registerId)
{
    return regs[checkRegisterId(registerId)];
}

void Registers::setRegister(uint8_t registerId, uint32_t registerValue)
{
    regs[checkRegisterId(registerId)] = registerValue;    
}
