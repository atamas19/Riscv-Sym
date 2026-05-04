#pragma once

#include "Instruction.h"

namespace BType
{

struct InstructionArguments {
    const int32_t imm;
    const uint8_t rs1;
    const uint8_t rs2;
};

namespace Instruction
{
    constexpr uint8_t getInstructionDescription() { return 0x63; }

    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput);

    // Branch Equal
    namespace BEQ {
        constexpr uint8_t getInstructionDescription() { return 0x0; }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Branch Not Equal
    namespace BNE {
        constexpr uint8_t getInstructionDescription() { return 0x1; }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Branch Less Than
    namespace BLT {
        constexpr uint8_t getInstructionDescription() { return 0x4; }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Branch Greater or Equal
    namespace BGE {
        constexpr uint8_t getInstructionDescription() { return 0x5; }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Branch Less Than Unsigned
    namespace BLTU {
        constexpr uint8_t getInstructionDescription() { return 0x6; }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Branch Greater or Equal Unsigned
    namespace BGEU {
        constexpr uint8_t getInstructionDescription() { return 0x7; }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }
} // namespace Instruction

} // namespace BType
