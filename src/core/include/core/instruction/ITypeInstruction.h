#pragma once

#include "Instruction.h"

#include <functional>

namespace IType
{

struct InstructionArguments {
    const int32_t imm;
    const uint8_t rs1;
    const uint8_t rd;
};

int32_t getImm(uint32_t encodedInstruction);

namespace ArithmeticInstruction
{
    constexpr uint8_t getInstructionDescription() { return 0x13; }

    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput);

    // Add Immediate
    namespace ADDI {
        constexpr uint8_t getInstructionDescription() { return 0x0; }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Set Less Than Immediate
    namespace SLTI {
        constexpr uint8_t getInstructionDescription() { return 0x2; }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Set Less Than Immediate Unsigned
    namespace SLTIU {
        constexpr uint8_t getInstructionDescription() { return 0x3; }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Exclusive Or Immediate
    namespace XORI {
        constexpr uint8_t getInstructionDescription() { return 0x4; }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Or Immediate
    namespace ORI {
        constexpr uint8_t getInstructionDescription() { return 0x6; }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // And Immediate
    namespace ANDI {
        constexpr uint8_t getInstructionDescription() { return 0x7; }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Shift Left Logical Immediate
    namespace SLLI {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x1, 0x0); }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Shit Right Logical Immediate
    namespace SRLI {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x5, 0x0); }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Shift Right Arithmetic Immediate
    namespace SRAI {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x5, 0x1); }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }
} // namespace ArithmeticInstruction

namespace LoadInstruction
{
    constexpr uint8_t getInstructionDescription() { return 0x3; }

    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput);

    // Load Byte
    namespace LB {
        constexpr uint8_t getInstructionDescription() { return 0x0; }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Load Halfword
    namespace LH {
        constexpr uint8_t getInstructionDescription() { return 0x1; }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Load Word
    namespace LW {
        constexpr uint8_t getInstructionDescription() { return 0x2; }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Load Byte Unsigned
    namespace LBU {
        constexpr uint8_t getInstructionDescription() { return 0x4; }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Load Halfword Unsigned
    namespace LHU {
        constexpr uint8_t getInstructionDescription() { return 0x5; }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }
} // namespace LoadInstruction

// Special case for JALR
// Jump And Link Register
namespace JALR {
    constexpr uint8_t getInstructionDescription() { return 0x67; }

    bool execute(const uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput);
} // namespace JALR

namespace FenceInstruction
{
    constexpr uint8_t getInstructionDescription() { return 0x0F; }

    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput);

    namespace FENCE {
        constexpr uint8_t getInstructionDescription() { return 0x0; }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    namespace FENCE_I {
        constexpr uint8_t getInstructionDescription() { return 0x1; }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }
} // namespace FenceInstruction

} // namespace IType
