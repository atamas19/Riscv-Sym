#include <core/instruction/ITypeInstruction.h>
#include <core/RiscvCpu.h>
#include <core/Memory.h>

#include <unordered_map>

#include <spdlog/fmt/fmt.h>

namespace IType
{

namespace {

int32_t getImm(uint32_t encodedInstruction) {
    int32_t imm = getBits(encodedInstruction, 20, 31);

    if (getBits(imm, 11, 11) == 1)
        imm |= 0xfffff000;
    else
        imm &= 0xfff;

    return imm;
}

InstructionArguments getInstructionArguments(uint32_t encodedInstruction) {
    const uint8_t rd  = getBits(encodedInstruction,  7, 11);
    const uint8_t rs1 = getBits(encodedInstruction, 15, 19);
    const int32_t imm = getImm(encodedInstruction);

    return {imm, rs1, rd};
}

} // namespace

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
                "Performed SLTI: x{} = (x{} ({}) < imm ({}))",
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

}

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
}

bool JALR_NEW::execute(const uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
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

}

void Instruction::decode()
{
    rd  = getBits(instruction,  7, 11);
    rs1 = getBits(instruction, 15, 19);
    imm = getBits(instruction, 20, 31);

    if (getBits(imm, 11, 11) == 1)
        imm |= 0xfffff000;
    else
        imm &= 0xfff;
}

// This create function will handle the IType instructions that have the 0x13 opcode
std::unique_ptr<Instruction> ArithmeticInstructionFactory::create(uint32_t encodedInstruction)
{
    static const std::unordered_map<uint8_t, std::function<std::unique_ptr<Instruction>(uint32_t)>> instructionMap = {
        { ADDI::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<ADDI> (ins); }},
        { SLTI::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<SLTI> (ins); }},
        { SLTIU::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<SLTIU>(ins); }},
        { XORI::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<XORI> (ins); }},
        { ORI::getInstructionDescriptor  (), [](uint32_t ins) { return std::make_unique<ORI>  (ins); }},
        { ANDI::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<ANDI> (ins); }}
    };

                                    // funct3, specialBit
    static const std::unordered_map<std::tuple<uint8_t, uint8_t>, std::function<std::unique_ptr<Instruction>(uint32_t)>, TupleHash> specialInstructionMap = {
        { SLLI::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<SLLI>(ins); }},
        { SRLI::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<SRLI>(ins); }},
        { SRAI::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<SRAI>(ins); }}
    };

    uint8_t funct3 = getBits(encodedInstruction, 12, 14);

    auto it = instructionMap.find(funct3);
    if (it != instructionMap.end())
        return it->second(encodedInstruction);
    else
    {
        uint8_t specialBit = getBits(encodedInstruction, 30, 30);
        std::tuple<uint8_t, uint8_t> specialInstructionDescriptor{funct3, specialBit};

        auto specialIt = specialInstructionMap.find(specialInstructionDescriptor);
        if (specialIt != specialInstructionMap.end())
            return specialIt->second(encodedInstruction);
    }

    return nullptr;
}

// Arithmetic instructions

void ADDI::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);

    uint32_t result = rs1Value + imm;

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed ADDI: x{} = x{} ({}) + imm ({}).",
            rd, rs1, rs1Value, imm
        );
        instructionOutput->setRegisters({rs1, rd});
    }
}

void SLTI::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    int32_t rs1Value = static_cast<int32_t>(cpu.getRegister(rs1));

    uint8_t result = (rs1Value < imm);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed SLTI: x{} = (x{} ({}) < imm ({}))",
            rd, rs1, rs1Value, imm
        );
        instructionOutput->setRegisters({rs1, rd});
    }
}

void SLTIU::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t u_imm = static_cast<uint32_t>(imm);

    uint8_t result = (rs1Value < u_imm);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed SLTIU: x{} = (x{} ({}) < imm ({}))",
            rd, rs1, rs1Value, u_imm
        );
        instructionOutput->setRegisters({rs1, rd});
    }
}

void XORI::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);

    uint32_t result = (rs1Value ^ imm);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed XORI: x{} = x{} ({}) ^ imm ({}).",
            rd, rs1, rs1Value, imm
        );
        instructionOutput->setRegisters({rs1, rd});
    }
}

void ORI::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);

    uint32_t result = (rs1Value | imm);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed ORI: x{} = x{} ({}) | imm ({}).",
            rd, rs1, rs1Value, imm
        );
        instructionOutput->setRegisters({rs1, rd});
    }
}

void ANDI::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);

    uint32_t result = (rs1Value & imm);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed ANDI: x{} = x{} ({}) & imm ({}).",
            rd, rs1, rs1Value, imm
        );
        instructionOutput->setRegisters({rs1, rd});
    }
}

void SLLI::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    int32_t shamt_i = getBits(imm, 0, 4);

    uint32_t result = (rs1Value << shamt_i);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed SLLI: x{} = x{} ({}) << shamt ({}).",
            rd, rs1, rs1Value, shamt_i
        );
        instructionOutput->setRegisters({rs1, rd});
    }
}

void SRLI::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    int32_t shamt_i = getBits(imm, 0, 4);

    uint32_t result = (static_cast<uint32_t>(rs1Value) >> shamt_i);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed SRLI: x{} = x{} ({}) >> shamt ({}).",
            rd, rs1, rs1Value, shamt_i
        );
        instructionOutput->setRegisters({rs1, rd});
    }
}

void SRAI::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    int32_t rs1Value = static_cast<int32_t>(cpu.getRegister(rs1));
    int32_t shamt_i = getBits(imm, 0, 4);

    uint32_t result = (rs1Value >> shamt_i);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed SRAI: x{} = x{} ({}) >> shamt ({}).",
            rd, rs1, rs1Value, shamt_i
        );
        instructionOutput->setRegisters({rs1, rd});
    }
}

// This create function will handle the IType instructions that are meant for loading memory, they have the 0x3 opcode
std::unique_ptr<Instruction> LoadInstructionFactory::create(uint32_t encodedInstruction)
{
    static const std::unordered_map<uint8_t, std::function<std::unique_ptr<Instruction>(uint32_t)>> instructionMap = {
        { LB::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<LB> (ins); }},
        { LH::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<LH> (ins); }},
        { LW::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<LW> (ins); }},
        { LBU::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<LBU>(ins); }},
        { LHU::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<LHU>(ins); }}
    };

    uint8_t funct3 = getBits(encodedInstruction, 12, 14);

    auto it = instructionMap.find(funct3);
    if (it != instructionMap.end())
        return it->second(encodedInstruction);

    return nullptr;
}

void LB::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);

    uint32_t addr = static_cast<uint32_t>(rs1Value) + static_cast<uint32_t>(imm);
    uint8_t rawByte = Memory::getInstance().read8(addr);
    int32_t signedVal = static_cast<int32_t>(static_cast<int8_t>(rawByte));

    cpu.setRegister(rd, static_cast<uint32_t>(signedVal));
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed LB: x{} = Mem[x{} ({}) + imm ({})] (byte loaded)",
            rd, rs1, rs1Value, imm
        );
        instructionOutput->setRegisters({rs1, rd});
        instructionOutput->setRamAddresses({{addr, rawByte}});
    }
}

void LH::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);

    uint32_t addr = static_cast<uint32_t>(rs1Value) + static_cast<uint32_t>(imm);
    uint16_t rawHw = Memory::getInstance().read16(addr);
    int32_t signedVal = static_cast<int32_t>(static_cast<int16_t>(rawHw));

    cpu.setRegister(rd, static_cast<uint32_t>(signedVal));
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed LH: x{} = Mem[x{} ({}) + imm ({})] (halfword loaded)",
            rd, rs1, rs1Value, imm
        );
        instructionOutput->setRegisters({rs1, rd});
        instructionOutput->setRamAddresses({{addr, rawHw}});
    }
}

void LW::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);

    uint32_t addr = static_cast<uint32_t>(rs1Value) + static_cast<uint32_t>(imm);
    uint32_t rawWord = Memory::getInstance().read32(addr);
    int32_t signedVal = static_cast<int32_t>(rawWord);

    cpu.setRegister(rd, static_cast<uint32_t>(signedVal));
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed LW: x{} = Mem[x{} ({}) + imm ({})] (word loaded)",
            rd, rs1, rs1Value, imm
        );
        instructionOutput->setRegisters({rs1, rd});
        instructionOutput->setRamAddresses({{addr, rawWord}});
    }
}

void LBU::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);

    uint32_t addr = static_cast<uint32_t>(rs1Value) + static_cast<uint32_t>(imm);
    uint32_t memoryValue = Memory::getInstance().read8(addr);

    cpu.setRegister(rd, memoryValue);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed LBU: x{} = Mem[x{} ({}) + imm ({})] (byte loaded, zero-extended)",
            rd, rs1, rs1Value, imm
        );
        instructionOutput->setRegisters({rs1, rd});
        instructionOutput->setRamAddresses({{addr, memoryValue}});
    }
}

void LHU::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);

    uint32_t addr = static_cast<uint32_t>(rs1Value) + static_cast<uint32_t>(imm);
    uint32_t memoryValue = Memory::getInstance().read16(addr);

    cpu.setRegister(rd, memoryValue);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed LHU: x{} = Mem[x{} ({}) + imm ({})] (halfword loaded, zero-extended)",
            rd, rs1, rs1Value, imm
        );
        instructionOutput->setRegisters({rs1, rd});
        instructionOutput->setRamAddresses({{addr, memoryValue}});
    }
}

void JALR::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);

    uint32_t targetAddress = (rs1Value + imm) & ~1;

    cpu.setRegister(rd, cpu.getPc() + 4);

    cpu.setPc(targetAddress);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed JALR: x{} = PC + 4, jumped to (x{} ({}) + imm ({})) & ~1 = {}",
            rd, rs1, rs1Value, imm, targetAddress
        );
        instructionOutput->setRegisters({rs1, rd});
    }
}

std::unique_ptr<Instruction> FenceInstructionFactory::create(uint32_t encodedInstruction)
{
    static const std::unordered_map<uint8_t, std::function<std::unique_ptr<Instruction>(uint32_t)>> instructionMap = {
        { FENCE  ::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<FENCE>  (ins); }},
        { FENCE_I::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<FENCE_I>(ins); }},
    };

    uint8_t funct3 = getBits(encodedInstruction, 12, 14);

    auto it = instructionMap.find(funct3);
    if (it != instructionMap.end())
        return it->second(encodedInstruction);

    return nullptr;
}

void FENCE::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput) {
    cpu.setPc(cpu.getPc() + 4);
    if (instructionOutput) {
        instructionOutput->consoleLog = "FENCE (Memory Barrier) - NOP";
    }
};

void FENCE_I::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput) {
    cpu.setPc(cpu.getPc() + 4);
    if (instructionOutput) {
        instructionOutput->consoleLog = "FENCE.I (Instruction Barrier) - NOP";
    }
}

} // namespace IType
