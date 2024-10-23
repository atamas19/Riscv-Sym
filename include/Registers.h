#pragma once

#include <cstdint>

class Registers
{
public:
    Registers();
    ~Registers() = default;

    const int8_t& checkRegisterId(const int8_t& registerId);

    int32_t getRegister(const int8_t& registerId);
    void setRegister(const int8_t& registerId, const int32_t& registerValue);
private:
    int32_t regs[32];
};