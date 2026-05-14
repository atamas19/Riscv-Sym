#include <core/instruction/ITypeInstruction.h>
#include <core/RiscvCpu.h>
#include <core/Memory.h>

#include <unordered_map>

#include <spdlog/fmt/fmt.h>

namespace IType
{

int32_t getImm(uint32_t encodedInstruction) {
    int32_t imm = getBits(encodedInstruction, 20, 31);

    if (getBits(imm, 11, 11) == 1)
        imm |= 0xfffff000;
    else
        imm &= 0xfff;

    return imm;
}

static InstructionArguments getInstructionArguments(uint32_t encodedInstruction) {
    const uint8_t rd  = getBits(encodedInstruction,  7, 11);
    const uint8_t rs1 = getBits(encodedInstruction, 15, 19);
    const int32_t imm = getImm(encodedInstruction);

    return {imm, rs1, rd};
}

namespace ArithmeticInstruction
{
    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const InstructionArguments instructionArguments = getInstructionArguments(encodedInstruction);

        const uint8_t funct3 = getBits(encodedInstruction, 12, 14);
        switch (funct3)
        {
        case ADDI::getInstructionDescription():
            return ADDI::execute(instructionArguments, cpu, instructionOutput);
        case SLTI::getInstructionDescription():
            return SLTI::execute(instructionArguments, cpu, instructionOutput);
        case SLTIU::getInstructionDescription():
            return SLTIU::execute(instructionArguments, cpu, instructionOutput);
        case XORI::getInstructionDescription():
            return XORI::execute(instructionArguments, cpu, instructionOutput);
        case ORI::getInstructionDescription():
            return ORI::execute(instructionArguments, cpu, instructionOutput);
        case ANDI::getInstructionDescription():
            return ANDI::execute(instructionArguments, cpu, instructionOutput);
        }

        const uint8_t specialBit = getBits(encodedInstruction, 30, 30);
        const uint16_t instructionDescriptor = createRuntimeInstructionDescription(funct3, specialBit);
        switch (instructionDescriptor)
        {
        case SLLI::getInstructionDescription():
            return SLLI::execute(instructionArguments, cpu, instructionOutput);
        case SRLI::getInstructionDescription():
            return SRLI::execute(instructionArguments, cpu, instructionOutput);
        case SRAI::getInstructionDescription():
            return SRAI::execute(instructionArguments, cpu, instructionOutput);
        }

        return false;
    }

    bool ADDI::execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
        const uint32_t result = rs1Value + instructionArguments.imm;

        cpu.setRegister(instructionArguments.rd, result);
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed ADDI: x{} = x{} ({}) + imm ({}).",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, instructionArguments.imm
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rd});
        }

        return true;
    }

    bool SLTI::execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const int32_t rs1Value = static_cast<int32_t>(cpu.getRegister(instructionArguments.rs1));
        const uint8_t result = (rs1Value < instructionArguments.imm);

        cpu.setRegister(instructionArguments.rd, result);
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "getBitsPerformed SLTI: x{} = (x{} ({}) < imm ({}))",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, instructionArguments.imm
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rd});
        }

        return true;
    }

    bool SLTIU::execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
        const uint32_t u_imm = static_cast<uint32_t>(instructionArguments.imm);
        const uint8_t result = (rs1Value < u_imm);

        cpu.setRegister(instructionArguments.rd, result);
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed SLTIU: x{} = (x{} ({}) < imm ({}))",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, u_imm
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rd});
        }

        return true;
    }

    bool XORI::execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
        uint32_t result = (rs1Value ^ instructionArguments.imm);

        cpu.setRegister(instructionArguments.rd, result);
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed XORI: x{} = x{} ({}) ^ imm ({}).",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, instructionArguments.imm
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rd});
        }

        return true;
    }

    bool ORI::execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
        const uint32_t result = (rs1Value | instructionArguments.imm);

        cpu.setRegister(instructionArguments.rd, result);
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed ORI: x{} = x{} ({}) | imm ({}).",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, instructionArguments.imm
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rd});
        }

        return true;
    }

    bool ANDI::execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);

        uint32_t result = (rs1Value & instructionArguments.imm);

        cpu.setRegister(instructionArguments.rd, result);
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed ANDI: x{} = x{} ({}) & imm ({}).",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, instructionArguments.imm
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rd});
        }

        return true;
    }

    bool SLLI::execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
        const int32_t shamt_i = getBits(instructionArguments.imm, 0, 4);
        const uint32_t result = (rs1Value << shamt_i);

        cpu.setRegister(instructionArguments.rd, result);
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed SLLI: x{} = x{} ({}) << shamt ({}).",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, shamt_i
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rd});
        }

        return true;
    }

    bool SRLI::execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
        const int32_t shamt_i = getBits(instructionArguments.imm, 0, 4);

        uint32_t result = (static_cast<uint32_t>(rs1Value) >> shamt_i);

        cpu.setRegister(instructionArguments.rd, result);
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed SRLI: x{} = x{} ({}) >> shamt ({}).",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, shamt_i
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rd});
        }

        return true;
    }

    bool SRAI::execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const int32_t rs1Value = static_cast<int32_t>(cpu.getRegister(instructionArguments.rs1));
        const int32_t shamt_i = getBits(instructionArguments.imm, 0, 4);
        const uint32_t result = (rs1Value >> shamt_i);

        cpu.setRegister(instructionArguments.rd, result);
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed SRAI: x{} = x{} ({}) >> shamt ({}).",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, shamt_i
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rd});
        }

        return true;
    }

} // namespace ArithmeticInstruction

namespace LoadInstruction
{
    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const InstructionArguments instructionArguments = getInstructionArguments(encodedInstruction);

        const uint8_t funct3 = getBits(encodedInstruction, 12, 14);
        switch (funct3)
        {
        case LB::getInstructionDescription():
            return LB::execute(instructionArguments, cpu, instructionOutput);
        case LH::getInstructionDescription():
            return LH::execute(instructionArguments, cpu, instructionOutput);
        case LW::getInstructionDescription():
            return LW::execute(instructionArguments, cpu, instructionOutput);
        case LBU::getInstructionDescription():
            return LBU::execute(instructionArguments, cpu, instructionOutput);
        case LHU::getInstructionDescription():
            return LHU::execute(instructionArguments, cpu, instructionOutput);
        }

        return false;
    }

    bool LB::execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);

        const uint32_t addr = static_cast<uint32_t>(rs1Value) + static_cast<uint32_t>(instructionArguments.imm);
        const uint8_t rawByte = Memory::getInstance().read8(addr);
        const int32_t signedVal = static_cast<int32_t>(static_cast<int8_t>(rawByte));

        cpu.setRegister(instructionArguments.rd, static_cast<uint32_t>(signedVal));
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed LB: x{} = Mem[x{} ({}) + imm ({})] (byte loaded)",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, instructionArguments.imm
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rd});
            instructionOutput->setRamAddresses({{addr, rawByte}});
        }

        return true;
    }

    bool LH::execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);

        const uint32_t addr = static_cast<uint32_t>(rs1Value) + static_cast<uint32_t>(instructionArguments.imm);
        const uint16_t rawHw = Memory::getInstance().read16(addr);
        const int32_t signedVal = static_cast<int32_t>(static_cast<int16_t>(rawHw));

        cpu.setRegister(instructionArguments.rd, static_cast<uint32_t>(signedVal));
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed LH: x{} = Mem[x{} ({}) + imm ({})] (halfword loaded)",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, instructionArguments.imm
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rd});
            instructionOutput->setRamAddresses({{addr, rawHw}});
        }

        return true;
    }

    bool LW::execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);

        const uint32_t addr = static_cast<uint32_t>(rs1Value) + static_cast<uint32_t>(instructionArguments.imm);
        const uint32_t rawWord = Memory::getInstance().read32(addr);
        const int32_t signedVal = static_cast<int32_t>(rawWord);

        cpu.setRegister(instructionArguments.rd, static_cast<uint32_t>(signedVal));
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed LW: x{} = Mem[x{} ({}) + imm ({})] (word loaded)",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, instructionArguments.imm
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rd});
            instructionOutput->setRamAddresses({{addr, rawWord}});
        }

        return true;
    }

    bool LBU::execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);

        const uint32_t addr = static_cast<uint32_t>(rs1Value) + static_cast<uint32_t>(instructionArguments.imm);
        const uint32_t memoryValue = Memory::getInstance().read8(addr);

        cpu.setRegister(instructionArguments.rd, memoryValue);
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed LBU: x{} = Mem[x{} ({}) + imm ({})] (byte loaded, zero-extended)",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, instructionArguments.imm
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rd});
            instructionOutput->setRamAddresses({{addr, memoryValue}});
        }

        return true;
    }

    bool LHU::execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);

        const uint32_t addr = static_cast<uint32_t>(rs1Value) + static_cast<uint32_t>(instructionArguments.imm);
        const uint32_t memoryValue = Memory::getInstance().read16(addr);

        cpu.setRegister(instructionArguments.rd, memoryValue);
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed LHU: x{} = Mem[x{} ({}) + imm ({})] (halfword loaded, zero-extended)",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, instructionArguments.imm
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rd});
            instructionOutput->setRamAddresses({{addr, memoryValue}});
        }

        return true;
    }

} // namespace LoadInstruction

bool JALR::execute(const uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
    const InstructionArguments instructionArguments = getInstructionArguments(encodedInstruction);
    const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
    const uint32_t targetAddress = (rs1Value + instructionArguments.imm) & ~1;

    cpu.setRegister(instructionArguments.rd, cpu.getPc() + 4);

    cpu.setPc(targetAddress);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed JALR: x{} = PC + 4, jumped to (x{} ({}) + imm ({})) & ~1 = {}",
            instructionArguments.rd, instructionArguments.rs1, rs1Value, instructionArguments.imm, targetAddress
        );
        instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rd});
    }

    return true;
}

namespace FenceInstruction
{
    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const InstructionArguments instructionArguments = getInstructionArguments(encodedInstruction);

        const uint8_t funct3 = getBits(encodedInstruction, 12, 14);
        switch (funct3)
        {
        case FENCE::getInstructionDescription():
            return FENCE::execute(instructionArguments, cpu, instructionOutput);
        case FENCE_I::getInstructionDescription():
            return FENCE_I::execute(instructionArguments, cpu, instructionOutput);
        }

        return false;
    }

    bool FENCE::execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        cpu.setPc(cpu.getPc() + 4);
        if (instructionOutput) {
            instructionOutput->consoleLog = "FENCE (Memory Barrier) - NOP";
        }

        return true;
    }

    bool FENCE_I::execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        cpu.setPc(cpu.getPc() + 4);
        if (instructionOutput) {
            instructionOutput->consoleLog = "FENCE.I (Instruction Barrier) - NOP";
        }

        return true;
    }

} // namespace FenceInstruction

} // namespace IType
