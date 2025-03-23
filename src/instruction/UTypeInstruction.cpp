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

void LUI::execute(RiscvCpu& cpu)
{
    int32_t imm_u = getImm_u();

    cpu.setRegister(rd, imm_u);
    cpu.setPc(cpu.getPc() + 4);

#if DEBUG
    std::cout << "rdValue: " << std::bitset<32>(cpu.getRegister(rd));
#endif
}

void AUIPC::execute(RiscvCpu& cpu)
{
    cpu.setPc(0x800012f4); // TODO: delete this, it was just for testing porpuses
    int32_t imm_u = getImm_u();
    int32_t resultValue = cpu.getPc() + imm_u;

    cpu.setRegister(rd, resultValue);
    cpu.setPc(cpu.getPc() + 4);

#if DEBUG
    std::cout << "rdValue: " << std::bitset<32>(cpu.getRegister(rd));
#endif
}

} // namespace UType
