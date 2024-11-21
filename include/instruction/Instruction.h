#pragma once

#include <cstdint>

#include <vector>

class RiscvCpu;

class Instruction
{
public:
    virtual void execute(RiscvCpu& cpu) = 0;
    virtual ~Instruction() = default;
protected:
    virtual void decode() = 0;
    uint32_t getBits(uint8_t x, uint8_t y);

    uint32_t instruction;
};
