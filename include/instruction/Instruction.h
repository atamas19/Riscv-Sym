#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <bitset>

class RiscvCpu;

uint32_t getBits(uint32_t instruction, uint8_t x, uint8_t y);

class Instruction
{
public:
    virtual void execute(RiscvCpu& cpu) = 0;
    virtual ~Instruction() = default;
protected:
    virtual void decode() = 0;

    uint32_t instruction;
};

class InstructionFactory
{
public:
    static std::unique_ptr<Instruction> create(uint32_t encodedInstruction);
};
