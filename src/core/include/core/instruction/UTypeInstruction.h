#pragma once

#include "Instruction.h"

namespace UType
{

struct InstructionArguments {
    const uint8_t rd;
    const int32_t imm;
};

namespace InstructionNew
{
    // Load Upper Immediate
    namespace LUI {
        constexpr uint8_t getInstructionDescription() { return 0x37; }

        bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Add Upper Immediate to Program Counter
    namespace AUIPC {
        constexpr uint8_t getInstructionDescription() { return 0x17; }

        bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }
}

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
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const uint8_t getInstructionDescriptor() { return 0x37; }
};

// Add Upper Immediate to Program Counter
class AUIPC : public Instruction
{
public:
    AUIPC(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const uint8_t getInstructionDescriptor() { return 0x17; }
};

} // namespace UType
