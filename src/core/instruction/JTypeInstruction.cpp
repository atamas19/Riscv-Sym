#include <core/instruction/JTypeInstruction.h>
#include <core/RiscvCpu.h>
#include <core/Memory.h>

#include <spdlog/fmt/fmt.h>

namespace JType
{

int32_t Instruction::getImm()
{
    int32_t tempValue = ((getBits(instruction, 31, 31) << 20) |
                         (getBits(instruction, 12, 19) << 12) |
                         (getBits(instruction, 20, 20) << 11) |
                         (getBits(instruction, 21, 30) << 1));

    if (getBits(tempValue, 20, 20) == 1)
        tempValue |= 0xfff00000;
    else
        tempValue &= 0x001fffff;

    return tempValue;
}

void Instruction::decode()
{
    rd  = getBits(instruction,  7, 11);
    imm = getImm();
}

void JAL::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t targetAddress = cpu.getPc() + imm;

    if (rd != 0)
        cpu.setRegister(rd, cpu.getPc() + 4);

    cpu.setPc(targetAddress);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format("Performed JAL: x{} = PC + 4, PC += {}.", rd, imm);
        instructionOutput->setRegisters({rd});
    }
}

} // namespace JType
