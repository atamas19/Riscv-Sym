#include "instruction/RTypeInstruction.h"
#include "RiscvCpu.h"

#include <unordered_map>
#include <iostream>
#include <bitset>

namespace RType
{

void Instruction::decode()
{
    std::cout << "Encoded instruction: " << std::bitset<32>(instruction) << std::endl;

    descriptor.setOpcode(getBits(instruction, 0, 6));
    descriptor.setFunct3(getBits(instruction, 12, 14));
    descriptor.setFunct7(getBits(instruction, 25, 31));

    rd  = getBits(instruction, 7, 11);
    rs1 = getBits(instruction, 15, 19);
    rs2 = getBits(instruction, 20, 24);

    std::cout << "rs1: " << std::bitset<8>(rs1) << std::endl;
    std::cout << "rs2: " << std::bitset<8>(rs2) << std::endl;
    std::cout << "rd: "  << std::bitset<8>(rd) << std::endl;

    // std::cout << std::bitset<8>(descriptor.getOpcode()) << "\n" << std::bitset<8>(descriptor.getFunct3()) << "\n" << std::bitset<8>(descriptor.getFunct7()) << "\n";
}

std::unique_ptr<Instruction> InstructionFactory::create(uint32_t encodedInstruction)
{
    static std::unordered_map<InstructionDescriptor, std::function<std::unique_ptr<Instruction>(uint32_t)>, InstructionDescriptor::InstructionDescriptorHash> instructionMap = {
        { {0x33, 0x0, 0x0}, [](uint32_t ins) { return std::make_unique<ADD>(ins); }}
    };

    uint8_t opcode = getBits(encodedInstruction, 0, 6);
    uint8_t funct3 = getBits(encodedInstruction, 12, 14);
    uint8_t funct7 = getBits(encodedInstruction, 25, 31);

    
    std::cout << "opcode: " << std::bitset<8>(opcode) << std::endl;
    std::cout << "funct3:  " << std::bitset<8>(funct3) << std::endl;
    std::cout << "funct7: "  << std::bitset<8>(funct7) << std::endl;

    InstructionDescriptor descriptor{opcode, funct3, funct7};

    auto it = instructionMap.find(descriptor);
    if (it != instructionMap.end())
        return it->second(encodedInstruction);

    return nullptr;
}

void ADD::execute(RiscvCpu& cpu)
{
    try
    {
        cpu.getRegisters().setRegister(2, 2);
        cpu.getRegisters().setRegister(3, 2);
        rd  = getBits(instruction, 7, 11);
        rs1 = getBits(instruction, 15, 19);
        rs2 = getBits(instruction, 20, 24);

        uint32_t rs1Value = cpu.getRegisters().getRegister(rs1);
        uint32_t rs2Value = cpu.getRegisters().getRegister(rs2);

        cpu.getRegisters().setRegister(rd, {rs1Value + rs2Value});

        std::cout << cpu.getRegisters().getRegister(rd);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

} // namespace RType