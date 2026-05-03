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

const uint16_t createRuntimeInstructionDescription(uint8_t funct3, uint8_t funct7) {
    return (static_cast<uint16_t>(funct3) << 8) | funct7;
}

bool Instruction_New::execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
    const uint8_t opcode = getBits(encodedInstruction, 0, 6);

    switch (opcode)
    {
    case RType::InstructionNew::getInstructionDescription():
        return RType::InstructionNew::execute(encodedInstruction, cpu, instructionOutput);
    case RType::AtomicNew::getInstructionDescription():
        return RType::AtomicNew::execute(encodedInstruction, cpu, instructionOutput);
    case BType::InstructionNew::getInstructionDescription():
        return BType::InstructionNew::execute(encodedInstruction, cpu, instructionOutput);
    case IType::ArithmeticInstruction::getInstructionDescription():
        return IType::ArithmeticInstruction::execute(encodedInstruction, cpu, instructionOutput);
    case IType::LoadInstruction::getInstructionDescription():
        return IType::LoadInstruction::execute(encodedInstruction, cpu, instructionOutput);
    case IType::JALR_NEW::getInstructionDescription():
        return IType::JALR_NEW::execute(encodedInstruction, cpu, instructionOutput);
    case IType::FenceInstruction::getInstructionDescription():
        return IType::FenceInstruction::execute(encodedInstruction, cpu, instructionOutput);
    case JType::InstructionNew::getInstructionDescription():
        return JType::InstructionNew::execute(encodedInstruction, cpu, instructionOutput);
    case SType::InstructionNew::getInstructionDescription():
        return SType::InstructionNew::execute(encodedInstruction, cpu, instructionOutput);
    default:
        break;
    }

    return false;
}

std::unique_ptr<Instruction> InstructionFactory::create(uint32_t encodedInstruction)
{
    static const std::unordered_map<uint8_t, std::function<std::unique_ptr<Instruction>(uint32_t)>> instructionMap = {
        { IType::ArithmeticInstructionFactory::getInstructionDescription(), [](uint32_t ins) { return IType::ArithmeticInstructionFactory::create(ins); }},
        { IType     ::FenceInstructionFactory::getInstructionDescription(), [](uint32_t ins) { return IType     ::FenceInstructionFactory::create(ins); }},
        { IType      ::LoadInstructionFactory::getInstructionDescription(), [](uint32_t ins) { return IType      ::LoadInstructionFactory::create(ins); }},

        { RType    ::AtomicInstructionFactory::getInstructionDescription(), [](uint32_t ins) { return RType    ::AtomicInstructionFactory::create(ins); }},
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
