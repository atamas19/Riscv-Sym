#pragma once

#include "Instruction.h"

namespace SType
{

struct InstructionArguments {
    const uint8_t rs1;
    const uint8_t rs2;
    const int32_t imm;
};

namespace InstructionNew
{
    constexpr uint8_t getInstructionDescription() { return 0x23; }

    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput);

    // Store Byte
    namespace SB {
        constexpr uint8_t getInstructionDescription() { return 0x0; }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Store Halfword
    namespace SH {
        constexpr uint8_t getInstructionDescription() { return 0x1; }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Store Word
    namespace SW {
        constexpr uint8_t getInstructionDescription() { return 0x2; }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }
}

class Instruction : public ::Instruction
{
public:
    Instruction(uint32_t instruction) { this->instruction = instruction; }

    virtual ~Instruction() = default;
protected:
    void decode() override;

    uint8_t rs1;
    uint8_t rs2;
    int32_t imm;
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
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const uint8_t getInstructionDescriptor() { return 0x0; }
};

// Store Halfword
class SH : public Instruction
{
public:
    SH(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const uint8_t getInstructionDescriptor() { return 0x1; }
};

// Store Word
class SW : public Instruction
{
public:
    SW(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const uint8_t getInstructionDescriptor() { return 0x2; }
};

} // namespace SType
