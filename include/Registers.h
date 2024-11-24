#pragma once

#include <cstdint>

class Registers
{
public:
    Registers() = default;
    ~Registers() = default;

    // Getters
    uint32_t getRegister(uint8_t registerId);

    // Setters
    void setRegister(uint8_t registerId, uint32_t registerValue);

private:
    uint32_t regs[32];
};