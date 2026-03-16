#include <core/instruction/RTypeInstruction.h>
#include <core/RiscvCpu.h>

#include <unordered_map>
#include <limits>

#include <fmt/core.h>

namespace RType
{

void Instruction::decode()
{
    rd  = getBits(instruction, 7, 11);
    rs1 = getBits(instruction, 15, 19);
    rs2 = getBits(instruction, 20, 24);
}

std::unique_ptr<Instruction> InstructionFactory::create(uint32_t encodedInstruction)
{
    static const std::unordered_map<InstructionDescriptor, std::function<std::unique_ptr<Instruction>(uint32_t)>, InstructionDescriptor::InstructionDescriptorHash> instructionMap = {
        { SLTU::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<SLTU>(ins); }},
        { ADD::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<ADD> (ins); }},
        { SUB::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<SUB> (ins); }},
        { SLL::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<SLL> (ins); }},
        { SLT::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<SLT> (ins); }},
        { XOR::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<XOR> (ins); }},
        { SRL::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<SRL> (ins); }},
        { SRA::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<SRA> (ins); }},
        { AND::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<AND> (ins); }},
        { OR::getInstructionDescriptor  (), [](uint32_t ins) { return std::make_unique<OR>  (ins); }},
        // RV32M instructions
        {    MUL::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<MUL>   (ins); }},
        {   MULH::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<MULH>  (ins); }},
        { MULHSU::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<MULHSU>(ins); }},
        {  MULHU::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<MULHU> (ins); }},
        {    DIV::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<DIV>   (ins); }},
        {   DIVU::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<DIVU>  (ins); }},
        {    REM::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<REM>   (ins); }},
        {   REMU::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<REMU>  (ins); }}
    };
    uint8_t funct3 = getBits(encodedInstruction, 12, 14);
    uint8_t funct7 = getBits(encodedInstruction, 25, 31);

    InstructionDescriptor descriptor{funct3, funct7};

    auto it = instructionMap.find(descriptor);
    if (it != instructionMap.end())
        return it->second(encodedInstruction);

    return nullptr;
}

void ADD::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);

    uint32_t result = rs2Value + rs1Value;

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed ADD: x" + std::to_string(rd) +
                                    " = x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                                    ") + x" + std::to_string(rs2) + " (" + std::to_string(rs2Value) + ").";
    instructionOutput.setRegisters({rs1, rs2, rd});
}

void SUB::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);

    uint32_t result = rs1Value - rs2Value;

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed SUB: x" + std::to_string(rd) +
                                    " = x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                                    ") - x" + std::to_string(rs2) + " (" + std::to_string(rs2Value) + ").";

    instructionOutput.setRegisters({rs1, rs2, rd});
}

void SLL::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);

    uint8_t shiftValue = getBits(rs2Value, 0, 5);

    uint32_t result = rs1Value << shiftValue;

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed SLL: x" + std::to_string(rd) +
                                    " = x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                                    ") << x" + std::to_string(rs2) + " (" + std::to_string(rs2Value) + " bits).";
    instructionOutput.setRegisters({rs1, rs2, rd});
}

void SLT::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    int32_t rs1Value = static_cast<int32_t>(cpu.getRegister(rs1));
    int32_t rs2Value = static_cast<int32_t>(cpu.getRegister(rs2));

    uint8_t result = (rs1Value < rs2Value);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed SLT: x" + std::to_string(rd) +
                                " = (x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                                ") < x" + std::to_string(rs2) + " (" + std::to_string(rs2Value) +
                                ")) → " + std::to_string(result) + ".";
    instructionOutput.setRegisters({rs1, rs2, rd});
}

void SLTU::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);

    uint8_t result = (rs1Value < rs2Value);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed SLTU: x" + std::to_string(rd) +
                                " = (unsigned)x" + std::to_string(rs1) + " (" + std::to_string(static_cast<uint32_t>(rs1Value)) +
                                ") < (unsigned)x" + std::to_string(rs2) + " (" + std::to_string(static_cast<uint32_t>(rs2Value)) +
                                ")) → " + std::to_string(result) + ".";
    instructionOutput.setRegisters({rs1, rs2, rd});
}

void XOR::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);

    uint32_t result = (rs1Value ^ rs2Value);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed XOR: x" + std::to_string(rd) +
                               " = x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                               ") ^ x" + std::to_string(rs2) + " (" + std::to_string(rs2Value) + ").";
    instructionOutput.setRegisters({rs1, rs2, rd});
}

void SRL::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);

    uint8_t shiftValue = getBits(rs2Value, 0, 5);

    uint32_t result = static_cast<uint32_t>(rs1Value) >> shiftValue;

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed SRL: x" + std::to_string(rd) +
                               " = x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                               ") >> x" + std::to_string(rs2) + " (" + std::to_string(shiftValue) + " bits) (logical shift).";
    instructionOutput.setRegisters({rs1, rs2, rd});
}

void SRA::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);

    uint8_t shiftValue = getBits(rs2Value, 0, 5);

    uint32_t result = static_cast<uint32_t>(static_cast<int32_t>(rs1Value) >> shiftValue);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed SRA: x" + std::to_string(rd) +
                               " = x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                               ") >> x" + std::to_string(rs2) + " (" + std::to_string(shiftValue) + " bits) (arithmetic shift).";
    instructionOutput.setRegisters({rs1, rs2, rd});
}

void OR::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);

    uint32_t result = (rs1Value | rs2Value);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed OR: x" + std::to_string(rd) +
                               " = x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                               ") | x" + std::to_string(rs2) + " (" + std::to_string(rs2Value) + ").";
    instructionOutput.setRegisters({rs1, rs2, rd});
}

void AND::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);

    uint32_t result = (rs1Value & rs2Value);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed AND: x" + std::to_string(rd) +
                               " = x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                               ") & x" + std::to_string(rs2) + " (" + std::to_string(rs2Value) + ").";
    instructionOutput.setRegisters({rs1, rs2, rd});
}

// RV32M instructions

void MUL::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) {
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);

    uint32_t result = rs1Value * rs2Value;

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = fmt::format(
        "Performed MUL: x{} = x{} ({}) * x{} ({})",
        rd, rs1, rs1Value, rs2, rs2Value
    );
    instructionOutput.setRegisters({rs1, rs2, rd});
}

void MULH::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) {
    int64_t rs1Value = static_cast<int32_t>(cpu.getRegister(rs1));
    int64_t rs2Value = static_cast<int32_t>(cpu.getRegister(rs2));

    int64_t result = rs1Value * rs2Value;

    cpu.setRegister(rd, static_cast<uint32_t>(result >> 32));
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = fmt::format(
        "Performed MULH: x{} = x{} ({}) * x{} ({}) [Upper 32 bits].",
        rd, rs1, rs1Value, rs2, rs2Value
    );
    instructionOutput.setRegisters({rs1, rs2, rd});
}

void MULHSU::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) {
    int64_t rs1Value = static_cast<int32_t>(cpu.getRegister(rs1));
    int64_t rs2Value = static_cast<int64_t>(cpu.getRegister(rs2));

    int64_t result = rs1Value * rs2Value;

    cpu.setRegister(rd, static_cast<uint32_t>(static_cast<uint64_t>(result) >> 32));
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = fmt::format(
        "Performed MULHSU: x{} = x{} ({}) * x{} ({}) [Upper 32 bits].",
        rd, rs1, rs1Value, rs2, rs2Value
    );
    instructionOutput.setRegisters({rs1, rs2, rd});
}

void MULHU::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) {
    uint64_t rs1Value = cpu.getRegister(rs1);
    uint64_t rs2Value = cpu.getRegister(rs2);

    uint64_t result = rs1Value * rs2Value;

    cpu.setRegister(rd, static_cast<uint32_t>(result >> 32));
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = fmt::format(
        "Performed MULHU: x{} = x{} ({}) * x{} ({}) [Upper 32 bits].",
        rd, rs1, rs1Value, rs2, rs2Value
    );
    instructionOutput.setRegisters({rs1, rs2, rd});
}

void DIV::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) {
    int32_t rs1Value = static_cast<int32_t>(cpu.getRegister(rs1));
    int32_t rs2Value = static_cast<int32_t>(cpu.getRegister(rs2));
    uint32_t result;

    if (rs2Value == 0) {
        result = 0xFFFFFFFF;
    } else if (rs1Value == std::numeric_limits<int32_t>::min() && rs2Value == -1) {
        result = static_cast<uint32_t>(rs1Value);
    } else {
        result = static_cast<uint32_t>(rs1Value / rs2Value);
    }

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = fmt::format(
        "Performed DIV: x{} = x{} ({}) / x{} ({})",
        rd, rs1, rs1Value, rs2, rs2Value
    );
    instructionOutput.setRegisters({rs1, rs2, rd});
}

void DIVU::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) {
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);
    uint32_t result;

    if (rs2Value == 0) {
        result = 0xFFFFFFFF;
    } else {
        result = rs1Value / rs2Value;
    }

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = fmt::format(
        "Performed DIVU: x{} = x{} ({}) / x{} ({})",
        rd, rs1, rs1Value, rs2, rs2Value
    );
    instructionOutput.setRegisters({rs1, rs2, rd});
}

void REM::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) {
    int32_t rs1Value = static_cast<int32_t>(cpu.getRegister(rs1));
    int32_t rs2Value = static_cast<int32_t>(cpu.getRegister(rs2));
    uint32_t result;

    if (rs2Value == 0) {
        result = static_cast<uint32_t>(rs1Value);
    } else if (rs1Value == std::numeric_limits<int32_t>::min() && rs2Value == -1) {
        result = 0;
    } else {
        result = static_cast<uint32_t>(rs1Value % rs2Value);
    }

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = fmt::format(
        "Performed REM: x{} = x{} ({}) % x{} ({})",
        rd, rs1, rs1Value, rs2, rs2Value
    );
    instructionOutput.setRegisters({rs1, rs2, rd});
}

void REMU::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) {
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);
    uint32_t result;

    if (rs2Value == 0) {
        result = rs1Value;
    } else {
        result = rs1Value % rs2Value;
    }

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = fmt::format(
        "Performed REMU: x{} = x{} ({}) % x{} ({})",
        rd, rs1, rs1Value, rs2, rs2Value
    );
    instructionOutput.setRegisters({rs1, rs2, rd});
}

// RV32A Instructions

std::unique_ptr<Instruction> AtomicInstructionFactory::create(uint32_t encodedInstruction)
{
    static const std::unordered_map<InstructionDescriptor, std::function<std::unique_ptr<Instruction>(uint32_t)>, InstructionDescriptor::InstructionDescriptorHash> instructionMap = {
        { AMOSWAP::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<AMOSWAP>(ins); }}
    };

    uint8_t funct3 = getBits(encodedInstruction, 12, 14);
    uint8_t funct7 = getBits(encodedInstruction, 25, 31);

    uint8_t amo_op = funct7 >> 2;

    InstructionDescriptor descriptor{funct3, amo_op};

    auto it = instructionMap.find(descriptor);
    if (it != instructionMap.end())
        return it->second(encodedInstruction);

    return nullptr;
}

void AMOSWAP::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    Memory& mem = Memory::getInstance();

    uint32_t memory_address = cpu.getRegister(rs1);
    uint32_t value_to_write = cpu.getRegister(rs2);

    uint32_t old_value = mem.read32(memory_address);
    mem.write32(memory_address, value_to_write);

    if (rd != 0) {
        cpu.setRegister(rd, old_value);
    }

    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed AMOSWAP.W: Mem[0x" + std::to_string(memory_address) +
                                   "] <- x" + std::to_string(rs2) + " (" + std::to_string(value_to_write) +
                                   "), old_val -> x" + std::to_string(rd);

    instructionOutput.setRegisters({rs1, rs2, rd});
}

} // namespace RType
