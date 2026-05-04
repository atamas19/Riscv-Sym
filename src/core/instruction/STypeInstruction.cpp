#include <core/instruction/STypeInstruction.h>
#include <core/RiscvCpu.h>
#include <core/Memory.h>

#include <unordered_map>
#include <functional>

#include <spdlog/fmt/fmt.h>

namespace SType
{

namespace Instruction
{
    namespace {
        int32_t getImm(uint32_t encodedInstruction) {
            int32_t imm = (getBits(encodedInstruction, 25, 31) << 5 | getBits(encodedInstruction, 7, 11));

            if (getBits(imm, 11, 11) == 1)
                imm = (imm | 0xfffff000);
            else
                imm = (imm & 0xfff);

            return imm;
        }

        InstructionArguments getInstructionArguments(uint32_t encodedInstruction) {
            const uint8_t rs1 = getBits(encodedInstruction, 15, 19);
            const uint8_t rs2 = getBits(encodedInstruction, 20, 24);
            const int32_t imm = getImm(encodedInstruction);

            return {imm, rs1, rs2};
        }

    } // namespace

    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const InstructionArguments instructionArguments = getInstructionArguments(encodedInstruction);

        const uint8_t funct3 = getBits(encodedInstruction, 12, 14);
        switch (funct3)
        {
        case SB::getInstructionDescription():
            return SB::execute(instructionArguments, cpu, instructionOutput);
        case SH::getInstructionDescription():
            return SH::execute(instructionArguments, cpu, instructionOutput);
        case SW::getInstructionDescription():
            return SW::execute(instructionArguments, cpu, instructionOutput);
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

} // namespace Instruction

} // namespace SType
