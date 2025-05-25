#include "AssemblyCompiler.h"

#include <unordered_map>
#include <cassert>
#include <functional>
#include <optional>
#include <iostream>

static std::optional<uint8_t> registerNameToNumber(const std::string& reg)
{
    if (reg.size() >= 2 && reg[0] == 'x')
    {
        try {
            unsigned long val = std::stoul(reg.substr(1));
            if (val <= 31)
                return static_cast<uint8_t>(val);
        } catch (...) {
            // Parsing failed
        }
    }
    return std::nullopt;
}

AssemblyCompiler& AssemblyCompiler::getInstance()
{
    static AssemblyCompiler instance;

    return instance;
}

void AssemblyCompiler::setInstructionOutput(InstructionOutput& instructionOutput)
{
    this->instructionOutput = &instructionOutput;
}

uint32_t AssemblyCompiler::compile(const std::string& asmCode, InstructionOutput& instructionOutput)
{
    auto& assemblyCompiler = AssemblyCompiler::getInstance();

    assemblyCompiler.setInstructionOutput(instructionOutput);

    uint32_t encodedInstruction = assemblyCompiler.getInstruction(asmCode);

    return encodedInstruction;
}
 // add x1, x2, x3 -> 01101110111011
 // addi x1, x2, 191
 // add x1, x2, 191(x3)
 // lw x1, 1
uint32_t AssemblyCompiler::getInstruction(const std::string& instructionString)
{
    static const std::unordered_map<std::string, std::function<uint32_t(const AssemblyInstruction&)>> instructionMap = {
        // RType instructions
        {"add",  [this](const AssemblyInstruction& ins) { return assembleADD(ins);  }},
        {"sub",  [this](const AssemblyInstruction& ins) { return assembleSUB(ins);  }},
        {"sll",  [this](const AssemblyInstruction& ins) { return assembleSLL(ins);  }},
        {"slt",  [this](const AssemblyInstruction& ins) { return assembleSLT(ins);  }},
        {"sltu", [this](const AssemblyInstruction& ins) { return assembleSLTU(ins); }},
        {"xor",  [this](const AssemblyInstruction& ins) { return assembleXOR(ins);  }},
        {"srl",  [this](const AssemblyInstruction& ins) { return assembleSRL(ins);  }},
        {"sra",  [this](const AssemblyInstruction& ins) { return assembleSRA(ins);  }},
        {"or",   [this](const AssemblyInstruction& ins) { return assembleOR(ins);   }},
        {"and",  [this](const AssemblyInstruction& ins) { return assembleAND(ins);  }}
    };
    AssemblyInstruction instruction{instructionString};

    auto it = instructionMap.find(instruction.getName());
    if (it != instructionMap.end())
        return it->second(instruction);
    else
    {
        instructionOutput->consoleLog = "Instruction is invalid or not implemented.";
        return 0;
    }
}

uint32_t AssemblyCompiler::encodeRType(uint8_t funct7, uint8_t rs2, uint8_t rs1, uint8_t funct3, uint8_t rd, uint8_t opcode)
{
    assert(funct7 <= 0x7F && "funct7 must be 7 bits");
    assert(funct3 <= 0x07 && "funct3 must be 3 bits");
    assert(opcode <= 0x7F && "opcode must be 7 bits");

    uint32_t encoded = 0;
    encoded |= (static_cast<uint32_t>(funct7) << 25);
    encoded |= (static_cast<uint32_t>(rs2)    << 20);
    encoded |= (static_cast<uint32_t>(rs1)    << 15);
    encoded |= (static_cast<uint32_t>(funct3) << 12);
    encoded |= (static_cast<uint32_t>(rd)     << 7);
    encoded |= static_cast<uint32_t>(opcode);

    return encoded;
}

uint32_t AssemblyCompiler::assembleRType(const AssemblyInstruction& instruction, uint8_t funct3, uint8_t funct7)
{
    auto& operands = instruction.getOperands();
    if (operands.size() != 3)
    {
        instructionOutput->consoleLog = "Not the right number of operands, should be exactly 3!";
        instructionOutput->exitCode = -1;
        return 0;
    }

    auto validateRegister = [&](const std::optional<uint8_t>& reg, const std::string& name) -> bool {
    if (!reg)
        {
            instructionOutput->consoleLog = name + " is not a valid register.";
            instructionOutput->exitCode = -1;
            return false;
        }
        return true;
    };

    auto rd = registerNameToNumber(operands.at(0));
    auto rs1 = registerNameToNumber(operands.at(1));
    auto rs2 = registerNameToNumber(operands.at(2));
    if (!validateRegister(rd, operands.at(0)) || !validateRegister(rs1, operands.at(1)) || !validateRegister(rs2, operands.at(2)))
        return 0;

    return encodeRType(funct7, *rs2, *rs1, funct3, *rd, 0x33);
}

uint32_t AssemblyCompiler::assembleADD(const AssemblyInstruction& instruction)
{
    uint8_t funct3 = 0x0;
    uint8_t funct7 = 0x0;

    return assembleRType(instruction, funct3, funct7);
}

uint32_t AssemblyCompiler::assembleSUB(const AssemblyInstruction& instruction)
{
    uint8_t funct3 = 0x0;
    uint8_t funct7 = 0x20;

    return assembleRType(instruction, funct3, funct7);
}

uint32_t AssemblyCompiler::assembleSLL(const AssemblyInstruction& instruction)
{
    uint8_t funct3 = 0x1;
    uint8_t funct7 = 0x0;

    return assembleRType(instruction, funct3, funct7);
}

uint32_t AssemblyCompiler::assembleSLT(const AssemblyInstruction& instruction)
{
    uint8_t funct3 = 0x2;
    uint8_t funct7 = 0x0;

    return assembleRType(instruction, funct3, funct7);
}

uint32_t AssemblyCompiler::assembleSLTU(const AssemblyInstruction& instruction)
{
    uint8_t funct3 = 0x3;
    uint8_t funct7 = 0x0;

    return assembleRType(instruction, funct3, funct7);
}

uint32_t AssemblyCompiler::assembleXOR(const AssemblyInstruction& instruction)
{
    uint8_t funct3 = 0x4;
    uint8_t funct7 = 0x0;

    return assembleRType(instruction, funct3, funct7);
}

uint32_t AssemblyCompiler::assembleSRL(const AssemblyInstruction& instruction)
{
    uint8_t funct3 = 0x5;
    uint8_t funct7 = 0x0;

    return assembleRType(instruction, funct3, funct7);
}

uint32_t AssemblyCompiler::assembleSRA(const AssemblyInstruction& instruction)
{
    uint8_t funct3 = 0x5;
    uint8_t funct7 = 0x20;

    return assembleRType(instruction, funct3, funct7);
}

uint32_t AssemblyCompiler::assembleOR(const AssemblyInstruction& instruction)
{
    uint8_t funct3 = 0x6;
    uint8_t funct7 = 0x0;

    return assembleRType(instruction, funct3, funct7);
}

uint32_t AssemblyCompiler::assembleAND(const AssemblyInstruction& instruction)
{
    uint8_t funct3 = 0x7;
    uint8_t funct7 = 0x0;

    return assembleRType(instruction, funct3, funct7);
}

void AssemblyInstruction::parse(const std::string& line)
{
    std::istringstream iss(line);
    iss >> instructionName;

    std::string operand;
    while (std::getline(iss, operand, ','))
    {
        trim(operand);
        if (!operand.empty())
            operands.push_back(operand);
    }
}

void AssemblyInstruction::print() const
{
    std::cout << "Instruction name: " << instructionName << "\nOperands: ";
    for (const auto& op : operands)
        std::cout << op << " ";
    std::cout << "\n";
}

void AssemblyInstruction::trim(std::string& s)
{
    size_t start = s.find_first_not_of(" \t");
    size_t end = s.find_last_not_of(" \t");
    s = (start == std::string::npos || end == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

const std::string& AssemblyInstruction::getName() const
{
    return instructionName;
}

const std::vector<std::string>& AssemblyInstruction::getOperands() const
{
    return operands;
}
