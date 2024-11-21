#pragma once

#include <cstdint>

class Registers
{
public:
    Registers()
    {
        regs[0] = 0;
    }
    ~Registers() = default;

    uint32_t getRegister(uint8_t registerId);
    void setRegister(uint8_t registerId, uint32_t registerValue);
private:
    uint8_t checkRegisterId(uint8_t registerId);

private:
    uint32_t regs[32];
};