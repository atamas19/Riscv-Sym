#include "instruction/STypeInstruction.h"
#include "RiscvCpu.h"
#include "Memory.h"

#include <unordered_map>
#include <iostream>
#include <functional>

namespace SType
{

void Instruction::decode()
{
    rs1 = getBits(instruction, 15, 19);
    rs2 = getBits(instruction, 20, 24);
    imm = (getBits(instruction, 25, 31) << 4 | getBits(instruction, 7, 11));

    if (getBits(imm, 11, 11) == 1)
        imm = (imm | 0xfffff000);
    else
        imm = (imm & 0xfff);

#if DEBUG
    std::cout << "rs1: "  << std::bitset<8>(rs1) << std::endl;
    std::cout << "rs2: "  << std::bitset<8>(rs2) << std::endl;
    std::cout << "imm: "  << std::bitset<8>(imm) << std::endl;
#endif
}

std::unique_ptr<Instruction> InstructionFactory::create(uint32_t encodedInstruction)
{
    static const std::unordered_map<uint8_t, std::function<std::unique_ptr<Instruction>(uint32_t)>> instructionMap = {
        { SB::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<SB>(ins); }},
        { SH::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<SH>(ins); }},
        { SW::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<SW>(ins); }}
    };

    uint8_t funct3 = getBits(encodedInstruction, 12, 14);

    auto it = instructionMap.find(funct3);
    if (it != instructionMap.end())
        return it->second(encodedInstruction);

    return nullptr;
}

void SB::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t rs2Value = cpu.getRegister(rs2);

    uint32_t addr = rs1Value + imm;
    uint32_t result = getBits(rs2Value, 0, 8);

    Memory::getInstance().write8(addr, result);

    cpu.setPc(cpu.getPc() + 4);

#if DEBUG
    std::cout << "Rezultat: " << Memory::getInstance().read8(addr);
#endif
}

void SH::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t rs2Value = cpu.getRegister(rs2);

    uint32_t addr = rs1Value + imm;
    uint32_t result = getBits(rs2Value, 0, 16);

    std::cout << "Rezultat inainte: " << Memory::getInstance().read16(addr);
    std::cout << "\nRezultat: " << result << "\n";

    Memory::getInstance().write16(addr, result);
    cpu.setPc(cpu.getPc() + 4);

#if DEBUG
    std::cout << "Rezultat: " << Memory::getInstance().read16(addr);
#endif
}

void SW::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t rs2Value = cpu.getRegister(rs2);

    uint32_t addr = rs1Value + imm;

    std::cout << "Rezultat inainte: " << Memory::getInstance().read32(addr);
    std::cout << "\nRezultat: " << rs2Value << "\n";

    Memory::getInstance().write32(addr, rs2Value);
    cpu.setPc(cpu.getPc() + 4);

#if DEBUG
    std::cout << "Rezultat: " << Memory::getInstance().read32(addr);
#endif
}

} // namespace SType