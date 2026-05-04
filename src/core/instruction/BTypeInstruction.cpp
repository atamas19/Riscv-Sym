#include <core/instruction/BTypeInstruction.h>
#include <core/RiscvCpu.h>
#include <core/Memory.h>

#include <unordered_map>
#include <functional>

#include <spdlog/fmt/fmt.h>

namespace BType
{

namespace Instruction
{
    namespace {
        int32_t getImm(uint32_t instruction) {
            int32_t imm = ( (getBits(instruction, 31, 31) << 12) |
                            (getBits(instruction, 7, 7)   << 11) |
                            (getBits(instruction, 25, 30) << 5)  |
                            (getBits(instruction, 8, 11)  << 1) );

            if (getBits(imm, 12, 12) == 1)
                imm |= 0xfffff000;
            else
                imm &= 0xfff;

            return imm;
        }

        InstructionArguments getInstructionArguments(uint32_t encodedInstruction) {
            const uint8_t rs1 = getBits(encodedInstruction, 15, 19);
            const uint8_t rs2 = getBits(encodedInstruction, 20, 24);
            const int32_t imm = getImm(encodedInstruction);

            return {imm, rs1, rs2};
        }
    } // namespace

    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const InstructionArguments instructionArguments = getInstructionArguments(encodedInstruction);

        const uint8_t funct3 = getBits(encodedInstruction, 12, 14);
        switch (funct3)
        {
        case BEQ::getInstructionDescription():
            return BEQ::execute(instructionArguments, cpu, instructionOutput);
        case BNE::getInstructionDescription():
            return BNE::execute(instructionArguments, cpu, instructionOutput);
        case BLT::getInstructionDescription():
            return BLT::execute(instructionArguments, cpu, instructionOutput);
        case BGE::getInstructionDescription():
            return BGE::execute(instructionArguments, cpu, instructionOutput);
        case BLTU::getInstructionDescription():
            return BLTU::execute(instructionArguments, cpu, instructionOutput);
        case BGEU::getInstructionDescription():
            return BGEU::execute(instructionArguments, cpu, instructionOutput);
        }

        return false;
    }

    bool BEQ::execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
        const uint32_t rs2Value = cpu.getRegister(instructionArguments.rs2);
        const int32_t pcIncrement = (rs1Value == rs2Value) ? instructionArguments.imm : 4;

        cpu.setPc(cpu.getPc() + pcIncrement);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed BEQ: x{} ({}) == x{} ({}) -> {}",
                instructionArguments.rs1, rs1Value, instructionArguments.rs2, rs2Value,
                (rs1Value == rs2Value) ? fmt::format("branch taken to PC + {}.", instructionArguments.imm) : "branch not taken."
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2});
        }

        return true;
    }

    bool BNE::execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
        const uint32_t rs2Value = cpu.getRegister(instructionArguments.rs2);
        const int32_t pcIncrement = (rs1Value != rs2Value) ? instructionArguments.imm : 4;

        cpu.setPc(cpu.getPc() + pcIncrement);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed BNE: x{} ({}) != x{} ({}) -> {}",
                instructionArguments.rs1, rs1Value, instructionArguments.rs2, rs2Value,
                (rs1Value != rs2Value) ? fmt::format("branch taken to PC + {}.", instructionArguments.imm) : "branch not taken."
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2});
        }

        return true;
    }

    bool BLT::execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const int32_t rs1Value = static_cast<int32_t>(cpu.getRegister(instructionArguments.rs1));
        const int32_t rs2Value = static_cast<int32_t>(cpu.getRegister(instructionArguments.rs2));
        const int32_t pcIncrement = (rs1Value < rs2Value) ? instructionArguments.imm : 4;

        cpu.setPc(cpu.getPc() + pcIncrement);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed BLT: x{} ({}) < x{} ({}) -> {}",
                instructionArguments.rs1, rs1Value, instructionArguments.rs2, rs2Value,
                (rs1Value < rs2Value) ? fmt::format("branch taken to PC + {}.", instructionArguments.imm) : "branch not taken."
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2});
        }
        return true;
    }

    bool BGE::execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const int32_t rs1Value = static_cast<int32_t>(cpu.getRegister(instructionArguments.rs1));
        const int32_t rs2Value = static_cast<int32_t>(cpu.getRegister(instructionArguments.rs2));
        const int32_t pcIncrement = (rs1Value >= rs2Value) ? instructionArguments.imm : 4;

        cpu.setPc(cpu.getPc() + pcIncrement);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed BGE: x{} ({}) >= x{} ({}) -> {}",
                instructionArguments.rs1, rs1Value, instructionArguments.rs2, rs2Value,
                (rs1Value >= rs2Value) ? fmt::format("branch taken to PC + {}.", instructionArguments.imm) : "branch not taken."
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2});
        }
        return true;
    }

    bool BLTU::execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
        const uint32_t rs2Value = cpu.getRegister(instructionArguments.rs2);
        const int32_t pcIncrement = (rs1Value < rs2Value) ? instructionArguments.imm : 4;

        cpu.setPc(cpu.getPc() + pcIncrement);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed BLTU: x{} ({}) < x{} ({}) -> {}",
                instructionArguments.rs1, rs1Value, instructionArguments.rs2, rs2Value,
                (rs1Value < rs2Value) ? fmt::format("branch taken to PC + {}.", instructionArguments.imm) : "branch not taken."
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2});
        }
        return true;
    }

    bool BGEU::execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
        const uint32_t rs2Value = cpu.getRegister(instructionArguments.rs2);
        const int32_t pcIncrement = (rs1Value >= rs2Value) ? instructionArguments.imm : 4;

        cpu.setPc(cpu.getPc() + pcIncrement);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed BGEU: x{} ({}) >= x{} ({}) -> {}",
                instructionArguments.rs1, rs1Value, instructionArguments.rs2, rs2Value,
                (rs1Value >= rs2Value) ? fmt::format("branch taken to PC + {}.", instructionArguments.imm) : "branch not taken."
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2});
        }
        return true;
    }
} // namespace Instruction

} // namespace BType
