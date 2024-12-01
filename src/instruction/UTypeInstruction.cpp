#include "instruction/UTypeInstruction.h"
#include "RiscvCpu.h"

#include <iostream>
#include <bitset>

namespace UType
{

void Instruction::decode()
{
    rd  = getBits(instruction, 7, 11);
    imm = getBits(instruction, 12, 31);

#ifdef DEBUG
    std::cout << "rd: "   << std::bitset<8>(rd)  << std::endl;
    std::cout << "imm: "  << std::bitset<8>(imm) << std::endl;
#endif
}

void LUI::execute(RiscvCpu& cpu)
{
    int32_t imm_u = getImm_u();

    cpu.setRegister(rd, imm_u);
    cpu.setPc(cpu.getPc() + 4);

    if (cpu.getRegister(rd) == 0x12345000)
        std::cout << "Success!\n";

#ifdef DEBUG
    std::cout << "rdValue: " << std::bitset<32>(cpu.getRegister(rd));
#endif
}

} // namespace UType
