#include <core/instruction/STypeInstruction.h>
#include <core/RiscvCpu.h>
#include <core/Memory.h>

#include <unordered_map>
#include <functional>

#include <spdlog/fmt/fmt.h>

namespace SType
{

namespace InstructionNew
{
    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint8_t rs1 = getBits(encodedInstruction, 15, 19);
        const uint8_t rs2 = getBits(encodedInstruction, 20, 24);
        int32_t imm = (getBits(encodedInstruction, 25, 31) << 5 | getBits(encodedInstruction, 7, 11));

        if (getBits(imm, 11, 11) == 1)
            imm = (imm | 0xfffff000);
        else
            imm = (imm & 0xfff);

        const uint8_t funct3 = getBits(encodedInstruction, 12, 14);

        switch (funct3)
        {
        case SB::getInstructionDescriptor():
            return SB::execute({rs1, rs2, imm}, cpu, instructionOutput);
        case SH::getInstructionDescriptor():
            return SH::execute({rs1, rs2, imm}, cpu, instructionOutput);
        case SW::getInstructionDescriptor():
            return SW::execute({rs1, rs2, imm}, cpu, instructionOutput);
        default:
            break;
        }

        return false;
    }

    bool SB::execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
        const uint32_t rs2Value = cpu.getRegister(instructionArguments.rs2);

        const uint32_t finalAddress = static_cast<uint32_t>(rs1Value) + static_cast<uint32_t>(instructionArguments.imm);
        const uint8_t result = static_cast<uint8_t>(rs2Value);

        cpu.notifyStore(finalAddress, 1);

        Memory::getInstance().write8(finalAddress, result);

        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed SB: mem[x{} ({}) + {}] = x{} ({}) [byte].",
                instructionArguments.rs1, rs1Value, instructionArguments.imm, instructionArguments.rs2, (rs2Value & 0xFF)
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2});
            instructionOutput->setRamAddresses({{finalAddress, result}});
        }
        return true;
    }

    bool SH::execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
        const uint32_t rs2Value = cpu.getRegister(instructionArguments.rs2);

        const uint32_t finalAddress = static_cast<uint32_t>(rs1Value) + static_cast<uint32_t>(instructionArguments.imm);
        const uint16_t result = static_cast<uint16_t>(rs2Value);

        cpu.notifyStore(finalAddress, 2);

        Memory::getInstance().write16(finalAddress, result);

        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed SH: mem[x{} ({}) + {}] = x{} ({}) [halfword].",
                instructionArguments.rs1, rs1Value, instructionArguments.imm, instructionArguments.rs2, (rs2Value & 0xFFFF)
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2});
            instructionOutput->setRamAddresses({{finalAddress, result}});
        }
        return true;
    }

    bool SW::execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
        const uint32_t rs2Value = cpu.getRegister(instructionArguments.rs2);

        const uint32_t finalAddress = static_cast<uint32_t>(rs1Value) + static_cast<uint32_t>(instructionArguments.imm);
        const uint32_t result = static_cast<uint32_t>(rs2Value);

        cpu.notifyStore(finalAddress, 4);

        Memory::getInstance().write32(finalAddress, result);

        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed SW: mem[x{} ({}) + {}] = x{} ({}) [word].",
                instructionArguments.rs1, rs1Value, instructionArguments.imm, instructionArguments.rs2, result
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2});
            instructionOutput->setRamAddresses({{finalAddress, result}});
        }
        return true;
    }

}

void Instruction::decode()
{
    rs1 = getBits(instruction, 15, 19);
    rs2 = getBits(instruction, 20, 24);
    imm = (getBits(instruction, 25, 31) << 5 | getBits(instruction, 7, 11));

    if (getBits(imm, 11, 11) == 1)
        imm = (imm | 0xfffff000);
    else
        imm = (imm & 0xfff);
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

void SB::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);

    uint32_t finalAddress = static_cast<uint32_t>(rs1Value) + static_cast<uint32_t>(imm);

    uint8_t result = static_cast<uint8_t>(rs2Value);

    cpu.notifyStore(finalAddress, 1);

    Memory::getInstance().write8(finalAddress, result);

    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed SB: mem[x{} ({}) + {}] = x{} ({}) [byte].",
            rs1, rs1Value, imm, rs2, (rs2Value & 0xFF)
        );
        instructionOutput->setRegisters({rs1, rs2});
        instructionOutput->setRamAddresses({{finalAddress, result}});
    }
}

void SH::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);

    uint32_t finalAddress = static_cast<uint32_t>(rs1Value) + static_cast<uint32_t>(imm);

    uint16_t result = static_cast<uint16_t>(rs2Value);

    cpu.notifyStore(finalAddress, 2);

    Memory::getInstance().write16(finalAddress, result);

    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed SH: mem[x{} ({}) + {}] = x{} ({}) [halfword].",
            rs1, rs1Value, imm, rs2, (rs2Value & 0xFFFF)
        );
        instructionOutput->setRegisters({rs1, rs2});
        instructionOutput->setRamAddresses({{finalAddress, result}});
    }
}

void SW::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);

    uint32_t finalAddress = static_cast<uint32_t>(rs1Value) + static_cast<uint32_t>(imm);

    uint32_t result = static_cast<uint32_t>(rs2Value);

    cpu.notifyStore(finalAddress, 4);

    Memory::getInstance().write32(finalAddress, result);

    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed SW: mem[x{} ({}) + {}] = x{} ({}) [word].",
            rs1, rs1Value, imm, rs2, rs2Value
        );
        instructionOutput->setRegisters({rs1, rs2});
        instructionOutput->setRamAddresses({{finalAddress, result}});
    }
}

} // namespace SType
