#include "instruction/UTypeInstruction.h"
#include "RiscvCpu.h"

#include <iostream>

namespace UType
{

void Instruction::decode()
{
    rd  = getBits(instruction, 7, 11);
    imm = getBits(instruction, 12, 31);

#if DEBUG
    std::cout << "rd: "   << std::bitset<8>(rd)  << std::endl;
    std::cout << "imm: "  << std::bitset<8>(imm) << std::endl;
#endif
}

void LUI::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    int32_t imm_u = getImm_u();

    cpu.setRegister(rd, imm_u);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed LUI: x" + std::to_string(rd) +
                               " = " + std::to_string(imm_u) + ".";
    instructionOutput.setRegisters({rd});

#if DEBUG
    std::cout << "rdValue: " << std::bitset<32>(cpu.getRegister(rd));
#endif
}

void AUIPC::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    int32_t imm_u = getImm_u();
    int32_t resultValue = cpu.getPc() + imm_u;
    int32_t pc = cpu.getPc();

    cpu.setRegister(rd, resultValue);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed AUIPC: x" + std::to_string(rd) +
                               " = PC (" + std::to_string(pc) + ") + " +
                               std::to_string(imm_u) + " = " + std::to_string(resultValue) + ".";
    instructionOutput.setRegisters({rd});

#if DEBUG
    std::cout << "rdValue: " << std::bitset<32>(cpu.getRegister(rd));
#endif
}

} // namespace UType
