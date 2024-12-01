#include "instruction/Instruction.h"
#include "instruction/RTypeInstruction.h"

#include <stdexcept>
#include <unordered_map>
#include <functional>

uint32_t getBits(uint32_t instruction, uint8_t x, uint8_t y) 
{
    if (x > y || y >= 32)
        throw std::out_of_range("Invalid bit range");

    uint32_t mask = (1 << (y - x + 1)) - 1;
    return (instruction >> x) & mask;
}

std::unique_ptr<Instruction> InstructionFactory::create(uint32_t encodedInstruction)
{
    static std::unordered_map<uint8_t, std::function<std::unique_ptr<Instruction>(uint32_t)>> instructionMap = {
        {0x33, [](uint32_t ins) { return RType::InstructionFactory::create(ins); }}
    };

    uint8_t opcode = getBits(encodedInstruction, 0, 6);

    auto it = instructionMap.find(opcode);
    if (it != instructionMap.end())
        return it->second(encodedInstruction);

    return nullptr;
}
