#pragma once

#include "ITypeInstruction.h"


namespace System {

struct InstructionArguments {
    const int32_t imm;
    const uint16_t csr_addr;
    const uint8_t rs1;
    const uint8_t rd;
};

namespace Instruction
{
    constexpr uint8_t getInstructionDescription() { return 0x73; }

    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput);

    /////////////////////////////////
    ////// ZICSR Instructions //////
    /////////////////////////////////

    // CSR Read/Write
    namespace CSRRW {
        constexpr uint8_t getInstructionDescription() { return 0x1; }

        bool execute(InstructionArguments instructionArguments, uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // CSR Read/Set
    namespace CSRRS {
        constexpr uint8_t getInstructionDescription() { return 0x2; }

        bool execute(InstructionArguments instructionArguments, uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // CSR Read/Clear
    namespace CSRRC {
        constexpr uint8_t getInstructionDescription() { return 0x3; }

        bool execute(InstructionArguments instructionArguments, uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // CSR Read/Write Immediate
    namespace CSRRWI {
        constexpr uint8_t getInstructionDescription() { return 0x5; }

        bool execute(InstructionArguments instructionArguments, uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // CSR Read/Set Immediate
    namespace CSRRSI {
        constexpr uint8_t getInstructionDescription() { return 0x6; }

        bool execute(InstructionArguments instructionArguments, uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // CSR Read/Clear Immediate
    namespace CSRRCI {
        constexpr uint8_t getInstructionDescription() { return 0x7; }

        bool execute(InstructionArguments instructionArguments, uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    ///////////////////////////////////////
    ////// Environment instructions //////
    ///////////////////////////////////////

    // Environment Call
    namespace ECALL {
        constexpr uint8_t getInstructionDescription() { return 0x0; }

        bool execute(RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Environment Break
    namespace EBREAK {
        constexpr uint8_t getInstructionDescription() { return 0x1; }

        bool execute(RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    //////////////////////////////////
    ////// Return instructions //////
    //////////////////////////////////

    // Machine-mode RETurn
    namespace MRET {
        constexpr uint16_t getInstructionDescription() { return 0x302; }

        bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Supervisor-mode RETurn
    namespace SRET {
        constexpr uint16_t getInstructionDescription() { return 0x102; }

        bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    namespace SFENCE_VMA {
        constexpr uint16_t getInstructionDescription() { return 0x09; }

        bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

} // namespace Instruction

} // namespace System
