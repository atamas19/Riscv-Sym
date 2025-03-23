#include "instruction/JTypeInstruction.h"
#include "RiscvCpu.h"
#include "Memory.h"

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

} // namespace JType