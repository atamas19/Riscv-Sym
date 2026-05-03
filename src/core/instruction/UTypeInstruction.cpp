#include <core/instruction/UTypeInstruction.h>
#include <core/RiscvCpu.h>

#include <spdlog/fmt/fmt.h>

namespace UType
{

namespace InstructionNew
{

    static InstructionArguments getInstructionArguments(uint32_t encodedInstruction) {
        const uint8_t rd = getBits(encodedInstruction, 7, 11);
        const int32_t imm = (encodedInstruction & 0xfffff000);

        return {imm, rd};
    }

    bool LUI::execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const InstructionArguments instructionArguments = getInstructionArguments(encodedInstruction);

        cpu.setRegister(instructionArguments.rd, instructionArguments.imm);
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed LUI: x{} = {}.",
                instructionArguments.rd, instructionArguments.imm
            );
            instructionOutput->setRegisters({instructionArguments.rd});
        }
        return true;
    }

    bool AUIPC::execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const InstructionArguments instructionArguments = getInstructionArguments(encodedInstruction);

        const uint32_t pc = cpu.getPc();
        const uint32_t resultValue = pc + instructionArguments.imm;

        cpu.setRegister(instructionArguments.rd, resultValue);
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed AUIPC: x{} = PC ({}) + {} = {}.",
                instructionArguments.rd, pc, instructionArguments.imm, resultValue
            );
            instructionOutput->setRegisters({instructionArguments.rd});
        }
        return true;
    }
}


void Instruction::decode()
{
    rd  = getBits(instruction, 7, 11);
    imm = (instruction & 0xfffff000);
}

void LUI::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    cpu.setRegister(rd, imm);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed LUI: x{} = {}.",
            rd, imm
        );
        instructionOutput->setRegisters({rd});
    }
}

void AUIPC::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t pc = cpu.getPc();
    uint32_t resultValue = pc + imm;

    cpu.setRegister(rd, resultValue);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed AUIPC: x{} = PC ({}) + {} = {}.",
            rd, pc, imm, resultValue
        );
        instructionOutput->setRegisters({rd});
    }
}

} // namespace UType
