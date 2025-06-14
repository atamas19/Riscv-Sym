#pragma once

#include "Instruction.h"

namespace UType
{

class Instruction : public ::Instruction
{
public:
    Instruction(uint32_t instruction) { this->instruction = instruction; }

    virtual ~Instruction() = default;
protected:
    void decode() override;

    uint8_t rd;
    int32_t imm;
};

// Load Upper Immediate
class LUI : public Instruction
{
public:
    LUI(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) override;

    static const uint8_t getInstructionDescriptor() { return 0x37; }
};

// Add Upper Immediate to Program Counter
class AUIPC : public Instruction
{
public:
    AUIPC(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) override;

    static const uint8_t getInstructionDescriptor() { return 0x17; }
};

} // namespace UType
