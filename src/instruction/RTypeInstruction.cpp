#include "instruction/RTypeInstruction.h"

void RTypeInstruction::decode()
{
    opcode = getBits(0, 6);
    rd = getBits(7, 11);
    funct3 = getBits(12, 14);
    rs1 = getBits(15, 19);
    rs2 = getBits(20, 24);
    funct7 = getBits(25, 31);
}

std::unique_ptr<Instruction> RTypeInstructionFactory::create(uint32_t encodedInstruction)
{
    uint8_t funct3 = (encodedInstruction >> 12) & 0x07;
    uint8_t funct7 = (encodedInstruction >> 25) & 0x7F;

    if (funct3 == 0x00 && funct7 == 0x00) 
        return std::make_unique<ADD>();
}
