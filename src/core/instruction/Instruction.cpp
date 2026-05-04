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

uint16_t createRuntimeInstructionDescription(uint8_t funct3, uint8_t funct7) {
    return (static_cast<uint16_t>(funct3) << 8) | funct7;
}

bool Instruction::execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
    const uint8_t opcode = getBits(encodedInstruction, 0, 6);

    switch (opcode)
    {
    case RType::Instruction::getInstructionDescription():
        return RType::Instruction::execute(encodedInstruction, cpu, instructionOutput);
    case RType::AtomicInstruction::getInstructionDescription():
        return RType::AtomicInstruction::execute(encodedInstruction, cpu, instructionOutput);
    case BType::Instruction::getInstructionDescription():
        return BType::Instruction::execute(encodedInstruction, cpu, instructionOutput);
    case IType::ArithmeticInstruction::getInstructionDescription():
        return IType::ArithmeticInstruction::execute(encodedInstruction, cpu, instructionOutput);
    case IType::LoadInstruction::getInstructionDescription():
        return IType::LoadInstruction::execute(encodedInstruction, cpu, instructionOutput);
    case IType::JALR::getInstructionDescription():
        return IType::JALR::execute(encodedInstruction, cpu, instructionOutput);
    case IType::FenceInstruction::getInstructionDescription():
        return IType::FenceInstruction::execute(encodedInstruction, cpu, instructionOutput);
    case JType::Instruction::getInstructionDescription():
        return JType::Instruction::execute(encodedInstruction, cpu, instructionOutput);
    case SType::Instruction::getInstructionDescription():
        return SType::Instruction::execute(encodedInstruction, cpu, instructionOutput);
    case System::Instruction::getInstructionDescription():
        return System::Instruction::execute(encodedInstruction, cpu, instructionOutput);
    case UType::Instruction::LUI::getInstructionDescription():
        return UType::Instruction::LUI::execute(encodedInstruction, cpu, instructionOutput);
    case UType::Instruction::AUIPC::getInstructionDescription():
        return UType::Instruction::AUIPC::execute(encodedInstruction, cpu, instructionOutput);
    }

    return false;
}

void InstructionOutput::setRegisters(std::initializer_list<uint8_t> regs) {
    modifiedRegisters.assign(regs.begin(), regs.end());
}

void InstructionOutput::setRamAddresses(std::initializer_list<MemoryCell> memoryCell) {
    modifiedRamAddresses.assign(memoryCell.begin(), memoryCell.end());
}
