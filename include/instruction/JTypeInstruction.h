#pragma once

#include "Instruction.h"

namespace JType
{

class Instruction : public ::Instruction
{
public:
    Instruction(uint32_t instruction) { this->instruction = instruction; }

    virtual ~Instruction() = default;
private:
    int32_t getImm();

protected:
    void decode() override;

    uint8_t rd;
    int32_t imm;
};

class JAL : public Instruction
{
public:
    JAL(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu) override {}

    // static const InstructionDescriptor getInstructionDescriptor() { return {0x7, 0x0}; }
};

} // namespace JType