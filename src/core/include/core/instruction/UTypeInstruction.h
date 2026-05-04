#pragma once

#include "Instruction.h"

namespace UType
{

struct InstructionArguments {
    const int32_t imm;
    const uint8_t rd;
};

namespace Instruction
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

} // namespace Instruction

} // namespace UType
