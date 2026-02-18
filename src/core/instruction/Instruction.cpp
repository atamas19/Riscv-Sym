#include <core/instruction/Instruction.h>
#include <core/instruction/RTypeInstruction.h>
#include <core/instruction/UTypeInstruction.h>
#include <core/instruction/ITypeInstruction.h>
#include <core/instruction/STypeInstruction.h>
#include <core/instruction/BTypeInstruction.h>
#include <core/instruction/JTypeInstruction.h>
#include <core/instruction/SystemInstruction.h>

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
    static const std::unordered_map<uint8_t, std::function<std::unique_ptr<Instruction>(uint32_t)>> instructionMap = {
        { IType::ArithmeticInstructionFactory::getInstructionDescription(), [](uint32_t ins) { return IType::ArithmeticInstructionFactory::create(ins); }},
        { IType      ::LoadInstructionFactory::getInstructionDescription(), [](uint32_t ins) { return IType      ::LoadInstructionFactory::create(ins); }},
        { RType          ::InstructionFactory::getInstructionDescription(), [](uint32_t ins) { return RType          ::InstructionFactory::create(ins); }},
        { SType          ::InstructionFactory::getInstructionDescription(), [](uint32_t ins) { return SType          ::InstructionFactory::create(ins); }},
        { BType          ::InstructionFactory::getInstructionDescription(), [](uint32_t ins) { return BType          ::InstructionFactory::create(ins); }},
        { System         ::InstructionFactory::getInstructionDescription(), [](uint32_t ins) { return System         ::InstructionFactory::create(ins); }},

        { UType::LUI::getInstructionDescriptor  (), [](uint32_t ins) { return std::make_unique<UType::LUI>  (ins); }},
        { UType::AUIPC::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<UType::AUIPC>(ins); }},
        { IType::JALR::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<IType::JALR> (ins); }},
        { JType::JAL::getInstructionDescriptor  (), [](uint32_t ins) { return std::make_unique<JType::JAL>  (ins); }}
    };

    uint8_t opcode = getBits(encodedInstruction, 0, 6);

    auto it = instructionMap.find(opcode);
    if (it != instructionMap.end())
        return it->second(encodedInstruction);

    return nullptr;
}

void InstructionOutput::setRegisters(std::initializer_list<uint8_t> regs)
{
    modifiedRegisters.assign(regs.begin(), regs.end());
}

void InstructionOutput::setRamAddresses(std::initializer_list<MemoryCell> memoryCell)
{
    modifiedRamAddresses.assign(memoryCell.begin(), memoryCell.end());
}
