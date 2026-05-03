#pragma once

#include "Instruction.h"

namespace JType
{

struct InstructionArguments {
    uint8_t rd;
    int32_t imm;
};

namespace InstructionNew
{
    constexpr uint8_t getInstructionDescription() { return 0x6f; }

    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput);

    // Jump and Link
    namespace JAL {
        // Doesn't matter since JAL is the only instruction in the J-type format, but it's here for consistency
        // constexpr uint8_t getInstructionDescriptor() { return 0x6f; }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }
}

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
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const uint8_t getInstructionDescriptor() { return 0x6f; }
};

} // namespace JType