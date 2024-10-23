#include "Registers.h"

#include <stdexcept>

Registers::Registers()
{
    regs[0] = 0;
}

const int8_t& Registers::checkRegisterId(const int8_t& registerId)
{
    if (registerId > 0 && registerId < 32)
        return registerId;
    else
        throw std::out_of_range("Register id out of bounds");
}

int32_t Registers::getRegister(const int8_t& registerId)
{
    return regs[checkRegisterId(registerId)];
}

void Registers::setRegister(const int8_t& registerId, const int32_t& registerValue)
{
    regs[checkRegisterId(registerId)] = registerValue;    
}
