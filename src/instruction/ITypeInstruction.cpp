#include "instruction/ITypeInstruction.h"
#include "RiscvCpu.h"

#include <iostream>
#include <unordered_map>
#include <bitset>

namespace IType
{

void Instruction::decode()
{
    rd  = getBits(instruction,  7, 11);
    rs1 = getBits(instruction, 15, 19);
    imm = getBits(instruction, 20, 31);

#ifdef DEBUG
    std::cout << "rd: "   << std::bitset<8>(rd)  << std::endl;
    std::cout << "imm: "  << std::bitset<8>(imm) << std::endl;
#endif
}

const int32_t Instruction::getImm_i() const
{
    int32_t imm_i{imm};

    if (getBits(imm_i, 11, 11) == 1)
        imm_i = (imm_i | 0xfffff000);
    else
        imm_i = (imm_i & 0xfff);
    return imm_i;
}

// This create function will handle the IType instructions that have the 0x13 opcode
std::unique_ptr<Instruction> ArithmeticInstructionFactory::create(uint32_t encodedInstruction)
{
    static std::unordered_map<uint8_t, std::function<std::unique_ptr<Instruction>(uint32_t)>> instructionMap = {
        { ADDI::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<ADDI> (ins); }},
        { SLTI::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<SLTI> (ins); }},
        { SLTIU::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<SLTIU>(ins); }},
        { XORI::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<XORI> (ins); }},
        { ORI::getInstructionDescriptor  (), [](uint32_t ins) { return std::make_unique<ORI>  (ins); }},
        { ANDI::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<ANDI> (ins); }}
    };

    uint8_t funct3 = getBits(encodedInstruction, 12, 14);

    auto it = instructionMap.find(funct3);
    if (it != instructionMap.end())
        return it->second(encodedInstruction);

    return nullptr;
}

// This create function will handle the IType instructions that have the 0x3 opcode
std::unique_ptr<Instruction> LoadInstructionFactory::create(uint32_t encodedInstruction)
{
    static std::unordered_map<uint8_t, std::function<std::unique_ptr<Instruction>(uint32_t)>> instructionMap = {
        // { ADDI::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<ADDI>(ins); }}
    };

    uint8_t funct3 = getBits(encodedInstruction, 12, 14);

    auto it = instructionMap.find(funct3);
    if (it != instructionMap.end())
        return it->second(encodedInstruction);

    return nullptr;
}

// Functions that have the opcode 0x13:

void ADDI::execute(RiscvCpu& cpu)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t imm_i = getImm_i();

    int32_t result = rs1Value + imm_i;

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

#ifdef DEBUG
    std::cout << cpu.getRegister(rd);
#endif
}

void SLTI::execute(RiscvCpu& cpu)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t imm_i = getImm_i();

    int8_t result = (rs1Value < imm_i);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

#ifdef DEBUG
    std::cout << cpu.getRegister(rd);
#endif
}

void SLTIU::execute(RiscvCpu& cpu)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t imm_i = getImm_i();

    int8_t result = (static_cast<uint32_t>(rs1Value) < static_cast<uint32_t>(imm_i));

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

#ifdef DEBUG
    std::cout << cpu.getRegister(rd);
#endif
}

void XORI::execute(RiscvCpu& cpu)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t imm_i = getImm_i();

    int32_t result = (rs1Value ^ imm_i);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

#ifdef DEBUG
    std::cout << cpu.getRegister(rd);
#endif
}

void ORI::execute(RiscvCpu& cpu)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t imm_i = getImm_i();

    int32_t result = (rs1Value | imm_i);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

#ifdef DEBUG
    std::cout << cpu.getRegister(rd);
#endif
}

void ANDI::execute(RiscvCpu& cpu)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t imm_i = getImm_i();

    int32_t result = (rs1Value & imm_i);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

#ifdef DEBUG
    std::cout << cpu.getRegister(rd);
#endif
}

} // namespace IType
