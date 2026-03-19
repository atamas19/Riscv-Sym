#include <core/instruction/ITypeInstruction.h>
#include <core/RiscvCpu.h>
#include <core/Memory.h>

#include <unordered_map>

#include <spdlog/fmt/fmt.h>

namespace IType
{

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

const int8_t Instruction::getShamt() const
{
    return getBits(imm, 0, 4);
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
    int32_t shamt_i = getShamt();

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
    int32_t shamt_i = getShamt();

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
    int32_t shamt_i = getShamt();

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
