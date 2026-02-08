#include <core/instruction/ITypeInstruction.h>
#include <core/RiscvCpu.h>
#include <core/Memory.h>

#include <iostream>
#include <unordered_map>

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

#if DEBUG
    std::cout << "rd: "   << std::bitset<8>(rd)  << std::endl;
    std::cout << "imm: "  << std::bitset<8>(imm) << std::endl;
#endif
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

void ADDI::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);

    uint32_t result = rs1Value + imm;

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed ADDI: x" + std::to_string(rd) +
                                    " = x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                                    ") + imm (" + std::to_string(imm) + ").";
    instructionOutput.setRegisters({rs1, rd});

#if DEBUG
    std::cout << cpu.getRegister(rd);
#endif
}

void SLTI::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);

    uint8_t result = (rs1Value < imm);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed SLTI: x" + std::to_string(rd) +
                                    " = (x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                                    ") < imm (" + std::to_string(imm) + "))";
    instructionOutput.setRegisters({rs1, rd});

#if DEBUG
    std::cout << cpu.getRegister(rd);
#endif
}

void SLTIU::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t u_rs1Value = static_cast<uint32_t>(rs1Value);
    uint32_t u_imm = static_cast<uint32_t>(imm);

    uint8_t result = (u_rs1Value < u_imm);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed SLTIU: x" + std::to_string(rd) +
                                    " = (x" + std::to_string(rs1) + " (" + std::to_string(u_rs1Value) +
                                    ") < imm (" + std::to_string(u_imm) + "))";
    instructionOutput.setRegisters({rs1, rd});

#if DEBUG
    std::cout << cpu.getRegister(rd);
#endif
}

void XORI::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);

    uint32_t result = (rs1Value ^ imm);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed XORI: x" + std::to_string(rd) +
                                    " = x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                                    ") ^ imm (" + std::to_string(imm) + ").";
    instructionOutput.setRegisters({rs1, rd});

#if DEBUG
    std::cout << cpu.getRegister(rd);
#endif
}

void ORI::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);

    uint32_t result = (rs1Value | imm);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed ORI: x" + std::to_string(rd) +
                                    " = x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                                    ") | imm (" + std::to_string(imm) + ").";
    instructionOutput.setRegisters({rs1, rd});

#if DEBUG
    std::cout << cpu.getRegister(rd);
#endif
}

void ANDI::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);

    uint32_t result = (rs1Value & imm);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed ANDI: x" + std::to_string(rd) +
                                    " = x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                                    ") & imm (" + std::to_string(imm) + ").";
    instructionOutput.setRegisters({rs1, rd});

#if DEBUG
    std::cout << cpu.getRegister(rd);
#endif
}

void SLLI::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    int32_t shamt_i = getShamt();

    uint32_t result = (rs1Value << shamt_i);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed SLLI: x" + std::to_string(rd) +
                                    " = x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                                    ") << shamt (" + std::to_string(shamt_i) + ").";
    instructionOutput.setRegisters({rs1, rd});

#if DEBUG
    std::cout << cpu.getRegister(rd);
#endif
}

void SRLI::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    int32_t shamt_i = getShamt();

    uint32_t result = (static_cast<uint32_t>(rs1Value) >> shamt_i);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed SRLI: x" + std::to_string(rd) +
                                    " = x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                                    ") >> shamt (" + std::to_string(shamt_i) + ").";
    instructionOutput.setRegisters({rs1, rd});

#if DEBUG
    std::cout << cpu.getRegister(rd);
#endif
}

void SRAI::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    int32_t shamt_i = getShamt();

    uint32_t result = (rs1Value >> shamt_i);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed SRAI: x" + std::to_string(rd) +
                                    " = x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                                    ") >> shamt (" + std::to_string(shamt_i) + ").";
    instructionOutput.setRegisters({rs1, rd});

#if DEBUG
    std::cout << cpu.getRegister(rd);
#endif
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

void LB::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);

    uint32_t addr = static_cast<uint32_t>(rs1Value) + static_cast<uint32_t>(imm);
    uint8_t rawByte = Memory::getInstance().read8(addr);
    int32_t signedVal = static_cast<int32_t>(static_cast<int8_t>(rawByte));

    cpu.setRegister(rd, static_cast<uint32_t>(signedVal));
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed LB: x" + std::to_string(rd) +
                                   " = Mem[x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                                   ") + imm (" + std::to_string(imm) + ")] (byte loaded)";
    instructionOutput.setRegisters({rs1, rd});
    instructionOutput.setRamAddresses({{addr, rawByte}});

#if DEBUG
    std::cout << "Address: " << addr << "\n";
    std::cout << cpu.getRegister(rd);
#endif
}

void LH::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);

    uint32_t addr = static_cast<uint32_t>(rs1Value) + static_cast<uint32_t>(imm);
    uint16_t rawHw = Memory::getInstance().read16(addr);
    int32_t signedVal = static_cast<int32_t>(static_cast<int16_t>(rawHw));

    cpu.setRegister(rd, static_cast<uint32_t>(signedVal));
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed LH: x" + std::to_string(rd) +
                                   " = Mem[x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                                   ") + imm (" + std::to_string(imm) + ")] (halfword loaded)";
    instructionOutput.setRegisters({rs1, rd});
    instructionOutput.setRamAddresses({{addr, rawHw}});

#if DEBUG
    std::cout << "Address: " << addr << "\n";
    std::cout << cpu.getRegister(rd);
#endif
}

void LW::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);

    uint32_t addr = static_cast<uint32_t>(rs1Value) + static_cast<uint32_t>(imm);
    uint32_t rawWord = Memory::getInstance().read32(addr);
    int32_t signedVal = static_cast<int32_t>(rawWord);

    cpu.setRegister(rd, static_cast<uint32_t>(signedVal));
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed LW: x" + std::to_string(rd) +
                                   " = Mem[x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                                   ") + imm (" + std::to_string(imm) + ")] (word loaded)";
    instructionOutput.setRegisters({rs1, rd});
    instructionOutput.setRamAddresses({{addr, rawWord}});

#if DEBUG
    std::cout << "Address: " << addr << "\n";
    std::cout << cpu.getRegister(rd);
#endif
}

void LBU::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);

    uint32_t addr = static_cast<uint32_t>(rs1Value) + static_cast<uint32_t>(imm);
    uint32_t memoryValue = Memory::getInstance().read8(addr);

    cpu.setRegister(rd, memoryValue);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed LBU: x" + std::to_string(rd) +
                                   " = Mem[x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                                   ") + imm (" + std::to_string(imm) + ")] (byte loaded, zero-extended)";
    instructionOutput.setRegisters({rs1, rd});
    instructionOutput.setRamAddresses({{addr, memoryValue}});

#if DEBUG
    std::cout << "Address: " << addr << "\n";
    std::cout << cpu.getRegister(rd);
#endif
}

void LHU::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);

    uint32_t addr = static_cast<uint32_t>(rs1Value) + static_cast<uint32_t>(imm);
    uint32_t memoryValue = Memory::getInstance().read16(addr);

    cpu.setRegister(rd, memoryValue);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed LHU: x" + std::to_string(rd) +
                                   " = Mem[x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
                                   ") + imm (" + std::to_string(imm) + ")] (halfword loaded, zero-extended)";
    instructionOutput.setRegisters({rs1, rd});
    instructionOutput.setRamAddresses({{addr, memoryValue}});

#if DEBUG
    std::cout << "Address: " << addr << "\n";
    std::cout << cpu.getRegister(rd);
#endif
}

void JALR::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);

    uint32_t targetAddress = (rs1Value + imm) & ~1;

    cpu.setRegister(rd, cpu.getPc() + 4);

    cpu.setPc(targetAddress);

    instructionOutput.consoleLog = "Performed JALR: x" + std::to_string(rd) +
        " = PC + 4, jumped to (x" + std::to_string(rs1) + " (" + std::to_string(rs1Value) +
        ") + imm (" + std::to_string(imm) + ")) & ~1 = " + std::to_string(targetAddress);
    instructionOutput.setRegisters({rs1, rd});

#if DEBUG
    std::cout << "Address: " << targetAddress << "\n";
    std::cout << cpu.getRegister(rd);
#endif
}

} // namespace IType
