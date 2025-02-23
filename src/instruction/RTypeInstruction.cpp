#include "instruction/RTypeInstruction.h"
#include "RiscvCpu.h"

#include <unordered_map>
#include <iostream>

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
    std::cout << "rd: "  << std::bitset<8>(rd)  << std::endl;
#endif
}

std::unique_ptr<Instruction> InstructionFactory::create(uint32_t encodedInstruction)
{
    static std::unordered_map<InstructionDescriptor, std::function<std::unique_ptr<Instruction>(uint32_t, InstructionDescriptor)>, InstructionDescriptor::InstructionDescriptorHash> instructionMap = {
        { SLTU::getInstructionDescriptor(), [](uint32_t ins, InstructionDescriptor descriptor) { return std::make_unique<SLTU>(ins, descriptor); }},
        { ADD::getInstructionDescriptor (), [](uint32_t ins, InstructionDescriptor descriptor) { return std::make_unique<ADD> (ins, descriptor); }},
        { SUB::getInstructionDescriptor (), [](uint32_t ins, InstructionDescriptor descriptor) { return std::make_unique<SUB> (ins, descriptor); }},
        { SLL::getInstructionDescriptor (), [](uint32_t ins, InstructionDescriptor descriptor) { return std::make_unique<SLL> (ins, descriptor); }},
        { SLT::getInstructionDescriptor (), [](uint32_t ins, InstructionDescriptor descriptor) { return std::make_unique<SLT> (ins, descriptor); }},
        { XOR::getInstructionDescriptor (), [](uint32_t ins, InstructionDescriptor descriptor) { return std::make_unique<XOR> (ins, descriptor); }},
        { SRL::getInstructionDescriptor (), [](uint32_t ins, InstructionDescriptor descriptor) { return std::make_unique<SRL> (ins, descriptor); }},
        { SRA::getInstructionDescriptor (), [](uint32_t ins, InstructionDescriptor descriptor) { return std::make_unique<SRA> (ins, descriptor); }},
        { AND::getInstructionDescriptor (), [](uint32_t ins, InstructionDescriptor descriptor) { return std::make_unique<AND> (ins, descriptor); }},
        { OR::getInstructionDescriptor  (), [](uint32_t ins, InstructionDescriptor descriptor) { return std::make_unique<OR>  (ins, descriptor); }}
    };

    uint8_t funct3 = getBits(encodedInstruction, 12, 14);
    uint8_t funct7 = getBits(encodedInstruction, 25, 31);

#ifdef DEBUG
    std::cout << "funct3: " << std::bitset<8>(funct3) << std::endl;
    std::cout << "funct7: " << std::bitset<8>(funct7) << std::endl;
#endif

    InstructionDescriptor descriptor{funct3, funct7};

    auto it = instructionMap.find(descriptor);
    if (it != instructionMap.end())
        return it->second(encodedInstruction, descriptor);

    return nullptr;
}

void ADD::execute(RiscvCpu& cpu)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t rs2Value = cpu.getRegister(rs2);

    int32_t result = rs2Value + rs1Value;

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

#ifdef DEBUG
    std::cout << cpu.getRegister(rd);
#endif
}

void SUB::execute(RiscvCpu& cpu)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t rs2Value = cpu.getRegister(rs2);

    int32_t result = rs2Value - rs1Value;

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

#ifdef DEBUG
    std::cout << cpu.getRegister(rd);
#endif
}

void SLL::execute(RiscvCpu& cpu)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t rs2Value = cpu.getRegister(rs2);

    uint8_t shiftValue = getBits(rs2Value, 0, 5);

    int32_t result = rs1Value << shiftValue;

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

#ifdef DEBUG
    std::cout << cpu.getRegister(rd);
#endif
}

void SLT::execute(RiscvCpu& cpu)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t rs2Value = cpu.getRegister(rs2);

    uint8_t result = (rs1Value < rs2Value);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

#ifdef DEBUG
    std::cout << cpu.getRegister(rd);
#endif
}

void SLTU::execute(RiscvCpu& cpu)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t rs2Value = cpu.getRegister(rs2);

    uint8_t result = (static_cast<uint32_t>(rs1Value) < static_cast<uint32_t>(rs2Value));

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

#ifdef DEBUG
    std::cout << cpu.getRegister(rd);
#endif
}

void XOR::execute(RiscvCpu& cpu)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t rs2Value = cpu.getRegister(rs2);

    int32_t result = (rs1Value ^ rs2Value);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

#ifdef DEBUG
    std::cout << cpu.getRegister(rd);
#endif
}

void SRL::execute(RiscvCpu& cpu)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t rs2Value = cpu.getRegister(rs2);

    uint8_t shiftValue = getBits(rs2Value, 0, 5);

    int32_t result = static_cast<uint32_t>(rs1Value) >> shiftValue;

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

#ifdef DEBUG
    std::cout << cpu.getRegister(rd);
#endif   
}

void SRA::execute(RiscvCpu& cpu)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t rs2Value = cpu.getRegister(rs2);

    uint8_t shiftValue = getBits(rs2Value, 0, 5);

    int32_t result = rs1Value >> shiftValue;

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

#ifdef DEBUG
    std::cout << cpu.getRegister(rd);
#endif   
}

void OR::execute(RiscvCpu& cpu)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t rs2Value = cpu.getRegister(rs2);

    int32_t result = (rs1Value | rs2Value);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

#ifdef DEBUG
    std::cout << cpu.getRegister(rd);
#endif   
}

void AND::execute(RiscvCpu& cpu)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t rs2Value = cpu.getRegister(rs2);

    int32_t result = (rs1Value & rs2Value);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

#ifdef DEBUG
    std::cout << cpu.getRegister(rd);
#endif   
}

} // namespace RType
