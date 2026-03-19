#include <core/instruction/UTypeInstruction.h>
#include <core/RiscvCpu.h>

#include <spdlog/fmt/fmt.h>

namespace UType
{

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
