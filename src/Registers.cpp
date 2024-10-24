#include "Registers.h"

#include <stdexcept>

const uint8_t& Registers::checkRegisterId(const uint8_t& registerId)
{
    if (registerId > 0 && registerId < 32)
        return registerId;
    else
        throw std::out_of_range("Register id out of bounds");
}

uint32_t Registers::getRegister(const uint8_t& registerId)
{
    return regs[checkRegisterId(registerId)];
}

void Registers::setRegister(const uint8_t& registerId, const uint32_t& registerValue)
{
    regs[checkRegisterId(registerId)] = registerValue;    
}
