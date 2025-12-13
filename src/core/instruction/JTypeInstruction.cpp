#include <core/instruction/JTypeInstruction.h>
#include <core/RiscvCpu.h>
#include <core/Memory.h>

#include <iostream>

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

#if DEBUG
    std::cout << "rd: "   << std::bitset<8>(rd)   << std::endl;
    std::cout << "imm: "  << std::bitset<32>(imm) << std::endl;
#endif
}

void JAL::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    uint32_t targetAddress = cpu.getPc() + imm;

    if (rd != 0)
        cpu.setRegister(rd, cpu.getPc() + 4);

    cpu.setPc(targetAddress);

    instructionOutput.consoleLog = "Performed JAL: x" + std::to_string(rd) +
        " = PC + 4, PC += " + std::to_string(imm) + ".";

    instructionOutput.setRegisters({rd});

#if DEBUG
    std::cout << "Address: " << targetAddress << "\n";
    std::cout << "Modified register value: " << cpu.getRegister(rd) << "\n";
    std::cout << "PC: " << cpu.getPc() << "\n";
#endif
}

} // namespace JType
