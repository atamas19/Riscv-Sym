#include <core/instruction/UTypeInstruction.h>
#include <core/RiscvCpu.h>

namespace UType
{

void Instruction::decode()
{
    rd  = getBits(instruction, 7, 11);
    imm = (instruction & 0xfffff000);
}

void LUI::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    cpu.setRegister(rd, imm);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed LUI: x" + std::to_string(rd) +
                               " = " + std::to_string(imm) + ".";
    instructionOutput.setRegisters({rd});
}

void AUIPC::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    uint32_t resultValue = cpu.getPc() + imm;
    uint32_t pc = cpu.getPc();

    cpu.setRegister(rd, resultValue);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed AUIPC: x" + std::to_string(rd) +
                               " = PC (" + std::to_string(pc) + ") + " +
                               std::to_string(imm) + " = " + std::to_string(resultValue) + ".";
    instructionOutput.setRegisters({rd});
}

} // namespace UType
