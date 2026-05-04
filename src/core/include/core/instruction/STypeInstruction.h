#pragma once

#include "Instruction.h"

namespace SType
{

struct InstructionArguments {
    const int32_t imm;
    const uint8_t rs1;
    const uint8_t rs2;
};

namespace Instruction
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

} // namespace Instruction

} // namespace SType
