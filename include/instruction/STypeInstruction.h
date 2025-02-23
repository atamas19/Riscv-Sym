#pragma once

#include "Instruction.h"

namespace SType
{

class Instruction : public ::Instruction
{
public:
    Instruction(uint32_t instruction) { this->instruction = instruction; }

    virtual ~Instruction() = default;
protected:
    void decode() override;

    uint8_t rs1;
    uint8_t rs2;
    int16_t imm;
};

class InstructionFactory
{
public:
    static std::unique_ptr<Instruction> create(uint32_t encodedInstruction);

    static const uint8_t getInstructionDescription() { return 0x23; }
};

// Store Byte
class SB : public Instruction
{
public:
    SB(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu) override;

    static const uint8_t getInstructionDescriptor() { return 0x0; }
};

// Store Halfword
class SH : public Instruction
{
public:
    SH(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu) override;

    static const uint8_t getInstructionDescriptor() { return 0x1; }
};

// Store Word
class SW : public Instruction
{
public:
    SW(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu) override;

    static const uint8_t getInstructionDescriptor() { return 0x2; }
};

} // namespace SType