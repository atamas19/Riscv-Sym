#include "instruction/BTypeInstruction.h"
#include "RiscvCpu.h"
#include "Memory.h"

#include <unordered_map>
#include <iostream>
#include <functional>

namespace BType
{

int32_t Instruction::getImm()
{
    int32_t tempValue = ((getBits(instruction, 31, 31) << 12) |
                         (getBits(instruction, 7, 7) << 11)   | 
                         (getBits(instruction, 25, 30) << 5)  | 
                         (getBits(instruction, 8, 11) << 1));

    if (getBits(tempValue, 12, 12) == 1)
        tempValue |= 0xfffff000;
    else
        tempValue &= 0xfff;

    return tempValue;
}

void Instruction::decode()
{
    rs1 = getBits(instruction, 15, 19);
    rs2 = getBits(instruction, 20, 24);
    imm = getImm();

#if DEBUG
    std::cout << "rs1: "  << std::bitset<8>(rs1) << std::endl;
    std::cout << "rs2: "  << std::bitset<8>(rs2) << std::endl;
    std::cout << "imm: "  << std::bitset<32>(imm) << std::endl;
#endif
}

std::unique_ptr<Instruction> InstructionFactory::create(uint32_t encodedInstruction)
{
    static std::unordered_map<uint8_t, std::function<std::unique_ptr<Instruction>(uint32_t)>> instructionMap = {
        { BEQ::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<BEQ> (ins); }},
        { BNE::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<BNE> (ins); }},
        { BLT::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<BLT> (ins); }},
        { BGE::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<BGE> (ins); }},
        { BLTU::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<BLTU>(ins); }},
        { BGEU::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<BGEU>(ins); }}
    };

    uint8_t funct3 = getBits(encodedInstruction, 12, 14);

    auto it = instructionMap.find(funct3);
    if (it != instructionMap.end())
        return it->second(encodedInstruction);

    return nullptr;
}

void BEQ::execute(RiscvCpu& cpu)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t rs2Value = cpu.getRegister(rs2);

    int32_t pcIncrement = (rs1Value == rs2Value) ? imm : 4;

    cpu.setPc(cpu.getPc() + pcIncrement);

#if DEBUG
    std::cout << "PC increment: " << pcIncrement << std::endl;
#endif
}

void BNE::execute(RiscvCpu& cpu)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t rs2Value = cpu.getRegister(rs2);

    int32_t pcIncrement = (rs1Value != rs2Value) ? imm : 4;

    cpu.setPc(cpu.getPc() + pcIncrement);

#if DEBUG
    std::cout << "PC increment: " << pcIncrement << std::endl;
#endif
}

void BLT::execute(RiscvCpu& cpu)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t rs2Value = cpu.getRegister(rs2);

    int32_t pcIncrement = (rs1Value < rs2Value) ? imm : 4;

    cpu.setPc(cpu.getPc() + pcIncrement);

#if DEBUG
    std::cout << "PC increment: " << pcIncrement << std::endl;
#endif
}

void BGE::execute(RiscvCpu& cpu)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t rs2Value = cpu.getRegister(rs2);

    int32_t pcIncrement = (rs1Value >= rs2Value) ? imm : 4;

    cpu.setPc(cpu.getPc() + pcIncrement);

#if DEBUG
    std::cout << "PC increment: " << pcIncrement << std::endl;
#endif
}

void BLTU::execute(RiscvCpu& cpu)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);

    int32_t pcIncrement = (rs1Value < rs2Value) ? imm : 4;

    cpu.setPc(cpu.getPc() + pcIncrement);

#if DEBUG
    std::cout << "PC increment: " << pcIncrement << std::endl;
#endif
}

void BGEU::execute(RiscvCpu& cpu)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);

    int32_t pcIncrement = (rs1Value >= rs2Value) ? imm : 4;

    cpu.setPc(cpu.getPc() + pcIncrement);

#if DEBUG
    std::cout << "PC increment: " << pcIncrement << std::endl;
#endif
}

} // namespace BType
