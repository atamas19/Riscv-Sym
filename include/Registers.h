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

    uint32_t getRegister(const uint8_t& registerId);
    void setRegister(const uint8_t& registerId, const uint32_t& registerValue);
private:
    const uint8_t& checkRegisterId(const uint8_t& registerId);

private:
    uint32_t regs[32];
};