#include "instruction/RTypeInstruction.h"
#include "RiscvCpu.h"

#include <unordered_map>
#include <iostream>

namespace RType
{

void Instruction::decode()
{
    rd  = getBits(instruction, 7, 11);
    rs1 = getBits(instruction, 15, 19);
    rs2 = getBits(instruction, 20, 24);

#if DEBUG
    std::cout << "rs1: " << std::bitset<8>(rs1) << std::endl;
    std::cout << "rs2: " << std::bitset<8>(rs2) << std::endl;
    std::cout << "rd: "  << std::bitset<8>(rd)  << std::endl;
#endif
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
        { OR::getInstructionDescriptor  (), [](uint32_t ins) { return std::make_unique<OR>  (ins); }}
    };

    uint8_t funct3 = getBits(encodedInstruction, 12, 14);
    uint8_t funct7 = getBits(encodedInstruction, 25, 31);

#if DEBUG
    std::cout << "funct3: " << std::bitset<8>(funct3) << std::endl;
    std::cout << "funct7: " << std::bitset<8>(funct7) << std::endl;
#endif

    InstructionDescriptor descriptor{funct3, funct7};

    auto it = instructionMap.find(descriptor);
    if (it != instructionMap.end())
        return it->second(encodedInstruction);

    return nullptr;
}

void ADD::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t rs2Value = cpu.getRegister(rs2);

    int32_t result = rs2Value + rs1Value;

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed ADD: x" + std::to_string(rd) +
                                    " = x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                                    ") + x" + std::to_string(rs2) + " (" + std::to_string(rs2Value) + ").";
    instructionOutput.setRegisters({rs1, rs2, rd});

#if DEBUG
    std::cout << cpu.getRegister(rd);
#endif
}

void SUB::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t rs2Value = cpu.getRegister(rs2);

    int32_t result = rs1Value - rs2Value;

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed SUB: x" + std::to_string(rd) +
                                    " = x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                                    ") - x" + std::to_string(rs2) + " (" + std::to_string(rs2Value) + ").";

    instructionOutput.setRegisters({rs1, rs2, rd});

#if DEBUG
    std::cout << cpu.getRegister(rd);
#endif
}

void SLL::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t rs2Value = cpu.getRegister(rs2);

    uint8_t shiftValue = getBits(rs2Value, 0, 5);

    int32_t result = rs1Value << shiftValue;

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed SLL: x" + std::to_string(rd) +
                                    " = x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                                    ") << x" + std::to_string(rs2) + " (" + std::to_string(rs2Value) + " bits).";
    instructionOutput.setRegisters({rs1, rs2, rd});

#if DEBUG
    std::cout << cpu.getRegister(rd);
#endif
}

void SLT::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t rs2Value = cpu.getRegister(rs2);

    uint8_t result = (rs1Value < rs2Value);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed SLT: x" + std::to_string(rd) +
                                " = (x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                                ") < x" + std::to_string(rs2) + " (" + std::to_string(rs2Value) +
                                ")) → " + std::to_string(result) + ".";
    instructionOutput.setRegisters({rs1, rs2, rd});

#if DEBUG
    std::cout << cpu.getRegister(rd);
#endif
}

void SLTU::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t rs2Value = cpu.getRegister(rs2);

    uint8_t result = (static_cast<uint32_t>(rs1Value) < static_cast<uint32_t>(rs2Value));

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed SLTU: x" + std::to_string(rd) +
                                " = (unsigned)x" + std::to_string(rs1) + " (" + std::to_string(static_cast<uint32_t>(rs1Value)) +
                                ") < (unsigned)x" + std::to_string(rs2) + " (" + std::to_string(static_cast<uint32_t>(rs2Value)) +
                                ")) → " + std::to_string(result) + ".";
    instructionOutput.setRegisters({rs1, rs2, rd});

#if DEBUG
    std::cout << cpu.getRegister(rd);
#endif
}

void XOR::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t rs2Value = cpu.getRegister(rs2);

    int32_t result = (rs1Value ^ rs2Value);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed XOR: x" + std::to_string(rd) +
                               " = x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                               ") ^ x" + std::to_string(rs2) + " (" + std::to_string(rs2Value) + ").";
    instructionOutput.setRegisters({rs1, rs2, rd});

#if DEBUG
    std::cout << cpu.getRegister(rd);
#endif
}

void SRL::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t rs2Value = cpu.getRegister(rs2);

    uint8_t shiftValue = getBits(rs2Value, 0, 5);

    int32_t result = static_cast<uint32_t>(rs1Value) >> shiftValue;

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed SRL: x" + std::to_string(rd) +
                               " = x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                               ") >> x" + std::to_string(rs2) + " (" + std::to_string(shiftValue) + " bits) (logical shift).";
    instructionOutput.setRegisters({rs1, rs2, rd});

#if DEBUG
    std::cout << cpu.getRegister(rd);
#endif   
}

void SRA::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t rs2Value = cpu.getRegister(rs2);

    uint8_t shiftValue = getBits(rs2Value, 0, 5);

    int32_t result = rs1Value >> shiftValue;

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed SRA: x" + std::to_string(rd) +
                               " = x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                               ") >> x" + std::to_string(rs2) + " (" + std::to_string(shiftValue) + " bits) (arithmetic shift).";
    instructionOutput.setRegisters({rs1, rs2, rd});

#if DEBUG
    std::cout << cpu.getRegister(rd);
#endif   
}

void OR::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t rs2Value = cpu.getRegister(rs2);

    int32_t result = (rs1Value | rs2Value);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed OR: x" + std::to_string(rd) +
                               " = x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                               ") | x" + std::to_string(rs2) + " (" + std::to_string(rs2Value) + ").";
    instructionOutput.setRegisters({rs1, rs2, rd});

#if DEBUG
    std::cout << cpu.getRegister(rd);
#endif   
}

void AND::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    int32_t rs1Value = cpu.getRegister(rs1);
    int32_t rs2Value = cpu.getRegister(rs2);

    int32_t result = (rs1Value & rs2Value);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed AND: x" + std::to_string(rd) +
                               " = x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                               ") & x" + std::to_string(rs2) + " (" + std::to_string(rs2Value) + ").";
    instructionOutput.setRegisters({rs1, rs2, rd});

#if DEBUG
    std::cout << cpu.getRegister(rd);
#endif   
}

} // namespace RType
