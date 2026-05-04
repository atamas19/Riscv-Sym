#pragma once

#include "Instruction.h"

#include <memory>
#include <functional>
#include <algorithm>

namespace RType
{

struct InstructionArguments {
    const uint8_t rs1;
    const uint8_t rs2;
    const uint8_t rd;
};

namespace Instruction
{
    constexpr uint8_t getInstructionDescription() { return 0x33; }

    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput);

    // Add
    namespace ADD {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x0, 0x0); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Substract
    namespace SUB {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x0, 0x20); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Shift Left Logical
    namespace SLL {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x1, 0x0); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Set Less Than
    namespace SLT {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x2, 0x0); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Set Less Than Unsigned
    namespace SLTU {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x3, 0x0); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Exclusive Or
    namespace XOR {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x4, 0x0); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Shit Right Logical
    namespace SRL {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x5, 0x0); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Shift Right Arithmetic
    namespace SRA {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x5, 0x20); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Or
    namespace OR {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x6, 0x0); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // And
    namespace AND {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x7, 0x0); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // RV32M instructions

    // Multiply
    namespace MUL {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x0, 0x1); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Multiply High
    namespace MULH {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x1, 0x1); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Multiply High Signed-Unsigned
    namespace MULHSU {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x2, 0x1); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Multiply High Unsigned
    namespace MULHU {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x3, 0x1); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Divide
    namespace DIV {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x4, 0x1); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Divide Unsigned
    namespace DIVU {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x5, 0x1); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Remainder
    namespace REM {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x6, 0x1); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Remainder Unsigned
    namespace REMU {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x7, 0x1); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

} // namespace Instruction

// RV32A instructions
namespace AtomicInstruction
{
    constexpr uint8_t getInstructionDescription() { return 0x2F; }

    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput);

    namespace LR {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x2, 0x2); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    namespace SC {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x2, 0x3); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    namespace AMO
    {
        struct InstructionArguments : public RType::InstructionArguments {
            std::string instructionName;
            std::function<uint32_t(uint32_t, uint32_t)> performAmoOperation;

            InstructionArguments(RType::InstructionArguments base,
                                std::string name,
                                std::function<uint32_t(uint32_t, uint32_t)> op)
                : RType::InstructionArguments(base),
                instructionName(std::move(name)),
                performAmoOperation(std::move(op))
            {}
        };

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);

        namespace SWAP {
            constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x2, 0x1); }

            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
        }

        namespace ADD {
            constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x2, 0x0); }

            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
        }

        namespace XOR {
            constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x2, 0x4); }

            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
        }

        namespace AND {
            constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x2, 0xC); }

            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
        }

        namespace OR {
            constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x2, 0x8); }

            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
        }

        namespace MIN {
            constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x2, 0x10); }

            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
        }

        namespace MAX {
            constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x2, 0x14); }

            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
        }

        namespace MINU {
            constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x2, 0x18); }

            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
        }

        namespace MAXU {
            constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x2, 0x1C); }

            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
        }

    } // namespace AMO

} // namespace AtomicInstruction

} // namespace RType
