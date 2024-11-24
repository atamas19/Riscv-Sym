#include "instruction/RTypeInstruction.h"

namespace RType
{

void Instruction::decode()
{
    descriptor.setOpcode(getBits(instruction, 0, 6));
    descriptor.setFunct3(getBits(instruction, 12, 14));
    descriptor.setFunct3(getBits(instruction, 25, 31));

    rd  = getBits(instruction, 7, 11);
    rs1 = getBits(instruction, 15, 19);
    rs2 = getBits(instruction, 20, 24);
}

std::unique_ptr<Instruction> InstructionFactory::create(uint32_t encodedInstruction)
{
    uint8_t opcode = getBits(encodedInstruction, 0, 6);
    uint8_t funct3 = getBits(encodedInstruction, 12, 14);
    uint8_t funct7 = getBits(encodedInstruction, 25, 31);

    InstructionDescriptor descriptor{opcode, funct3, funct7};

    std::vector<InstructionMapping> instructionMap = {
        { {0x33, 0x0, 0x0}, [](uint32_t ins) { return std::make_unique<ADD>(ins); }}
        // { {0x33, 0x0, 0x20}, [](uint32_t val) { return std::make_unique<SUBInstruction>(val); }}
    };

    if (opcode == 0x33 && funct3 == 0x00 && funct7 == 0x00)
        return std::make_unique<ADD>();
}

} // namespace RType