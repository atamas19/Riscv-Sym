#pragma once

#include "Instruction.h"

namespace JType
{

struct InstructionArguments {
    const int32_t imm;
    const uint8_t rd;
};

namespace Instruction
{
    constexpr uint8_t getInstructionDescription() { return 0x6f; }

    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput);

    // Jump and Link
    namespace JAL {
        // Doesn't matter since JAL is the only instruction in the J-type format, but it's here for consistency
        // constexpr uint8_t getInstructionDescription() { return 0x6f; }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }
} // namespace Instruction

} // namespace JType