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
        tempValue = (tempValue | 0xfffff000);
    else
        tempValue = (tempValue & 0xfff);

    return tempValue;
}

void Instruction::decode()
{
    rs1 = getBits(instruction, 15, 19);
    rs2 = getBits(instruction, 20, 24);
    imm = getImm();

#ifdef DEBUG
    std::cout << "rs1: "  << std::bitset<8>(rs1) << std::endl;
    std::cout << "rs2: "  << std::bitset<8>(rs2) << std::endl;
    std::cout << "imm: "  << std::bitset<8>(imm) << std::endl;
#endif
}

std::unique_ptr<Instruction> InstructionFactory::create(uint32_t encodedInstruction)
{
    static std::unordered_map<uint8_t, std::function<std::unique_ptr<Instruction>(uint32_t)>> instructionMap = {
        // { SB::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<SB>(ins); }}
    };

    uint8_t funct3 = getBits(encodedInstruction, 12, 14);

    auto it = instructionMap.find(funct3);
    if (it != instructionMap.end())
        return it->second(encodedInstruction);

    return nullptr;
}

// void SB::execute(RiscvCpu& cpu)
// {
//     int32_t rs1Value = cpu.getRegister(rs1);
//     int32_t rs2Value = cpu.getRegister(rs2);

//     uint32_t addr = rs1Value + imm;
//     uint32_t result = getBits(rs2Value, 0, 8);

//     Memory::getInstance().write8(addr, result);

//     cpu.setPc(cpu.getPc() + 4);

// #ifdef DEBUG
//     std::cout << "Rezultat: " << Memory::getInstance().read8(addr);
// #endif
// }

} // namespace BType
