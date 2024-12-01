#include "instruction/RTypeInstruction.h"
#include "RiscvCpu.h"

#include <unordered_map>
#include <iostream>
#include <bitset>

namespace RType
{

void Instruction::decode()
{
    rd  = getBits(instruction, 7, 11);
    rs1 = getBits(instruction, 15, 19);
    rs2 = getBits(instruction, 20, 24);

#ifdef DEBUG
    std::cout << "rs1: " << std::bitset<8>(rs1) << std::endl;
    std::cout << "rs2: " << std::bitset<8>(rs2) << std::endl;
    std::cout << "rd: "  << std::bitset<8>(rd) << std::endl;
#endif
}

std::unique_ptr<Instruction> InstructionFactory::create(uint32_t encodedInstruction)
{
    static std::unordered_map<InstructionDescriptor, std::function<std::unique_ptr<Instruction>(uint32_t, InstructionDescriptor)>, InstructionDescriptor::InstructionDescriptorHash> instructionMap = {
        { ADD::getInstructionDescriptor(), [](uint32_t ins, InstructionDescriptor descriptor) { return std::make_unique<ADD>(ins, descriptor); }},
        { SUB::getInstructionDescriptor(), [](uint32_t ins, InstructionDescriptor descriptor) { return std::make_unique<SUB>(ins, descriptor); }},
        { SLL::getInstructionDescriptor(), [](uint32_t ins, InstructionDescriptor descriptor) { return std::make_unique<SLL>(ins, descriptor); }}
    };

    uint8_t opcode = getBits(encodedInstruction, 0, 6);
    uint8_t funct3 = getBits(encodedInstruction, 12, 14);
    uint8_t funct7 = getBits(encodedInstruction, 25, 31);

#ifdef DEBUG
    std::cout << "opcode: " << std::bitset<8>(opcode) << std::endl;
    std::cout << "funct3: " << std::bitset<8>(funct3) << std::endl;
    std::cout << "funct7: " << std::bitset<8>(funct7) << std::endl;
#endif

    InstructionDescriptor descriptor{opcode, funct3, funct7};

    auto it = instructionMap.find(descriptor);
    if (it != instructionMap.end())
        return it->second(encodedInstruction, descriptor);

    return nullptr;
}

void ADD::execute(RiscvCpu& cpu)
{
    try
    {
        uint32_t rs1Value = cpu.getRegister(rs1);
        uint32_t rs2Value = cpu.getRegister(rs2);

        cpu.setRegister(rd, {rs1Value + rs2Value});
        cpu.setPc(cpu.getPc() + 4);

#ifdef DEBUG
        std::cout << cpu.getRegister(rd);
#endif
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void SUB::execute(RiscvCpu& cpu)
{
    try
    {
        uint32_t rs1Value = cpu.getRegister(rs1);
        uint32_t rs2Value = cpu.getRegister(rs2);

        cpu.setRegister(rd, {rs2Value - rs1Value});
        cpu.setPc(cpu.getPc() + 4);

#ifdef DEBUG
        std::cout << cpu.getRegister(rd);
#endif
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void SLL::execute(RiscvCpu& cpu)
{
    try
    {
        uint32_t rs1Value = cpu.getRegister(rs1);
        uint32_t rs2Value = cpu.getRegister(rs2);

        uint8_t shiftValue = getBits(rs2Value, 0, 5);

        rs1Value = rs1Value << shiftValue;

        cpu.setRegister(rd, rs1Value);
        cpu.setPc(cpu.getPc() + 4);

#ifdef DEBUG
        std::cout << cpu.getRegister(rd);
#endif
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

} // namespace RType