#include "AssemblyCompiler.h"

#include <unordered_map>
#include <cassert>
#include <functional>
#include <iostream>

static uint8_t registerNameToNumber(const std::string& reg)
{
    if (reg.size() >= 2 && reg[0] == 'x') {
        try {
            return std::stoi(reg.substr(1));
        } catch (...) {
            return -1;
        }
    }
    return -1;
}

AssemblyCompiler& AssemblyCompiler::getInstance()
{
    static AssemblyCompiler instance;

    return instance;
}

uint32_t AssemblyCompiler::compile(const std::string& asmCode)
{
    auto& assemblyCompiler = AssemblyCompiler::getInstance();

    uint32_t encodedInstruction = assemblyCompiler.getInstruction(asmCode);

    if (encodedInstruction != 0)
    {
        // means stuff is right
        return encodedInstruction;
    }
    else
    {
        // means stuff is not right
        return 0;
    }
}
 // add x1, x2, x3 -> 01101110111011
 // addi x1, x2, 191
 // add x1, x2, 191(x3)
 // lw x1, 1
uint32_t AssemblyCompiler::getInstruction(const std::string& instructionString)
{
    static const std::unordered_map<std::string, std::function<uint32_t(const AssemblyInstruction&)>> instructionMap = {
        {"add", [this](const AssemblyInstruction& ins) { return assembleAdd(ins); }}
    };
    AssemblyInstruction instruction{instructionString};

    auto it = instructionMap.find(instruction.getName());
    if (it != instructionMap.end())
        return it->second(instruction);
    else
        return 0;
}

uint32_t AssemblyCompiler::encodeRType(uint8_t funct7, uint8_t rs2, uint8_t rs1, uint8_t funct3, uint8_t rd, uint8_t opcode)
{
    assert(funct7 <= 0x7F && "funct7 must be 7 bits");
    assert(rs2    <= 0x1F && "rs2 must be 5 bits");
    assert(rs1    <= 0x1F && "rs1 must be 5 bits");
    assert(funct3 <= 0x07 && "funct3 must be 3 bits");
    assert(rd     <= 0x1F && "rd must be 5 bits");
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

uint32_t AssemblyCompiler::assembleAdd(const AssemblyInstruction& instruction)
{
    // TODO
    // Check if istringstream& is really the right type here, I should decode xy, xz, xw into real register numbers then pass everything into encodeRType
    // something like this
    auto& operands = instruction.getOperands();
    if (operands.size() != 3)
    {
        // Not the right number of operands
        return 0;
    }
    uint8_t rd = registerNameToNumber(operands.at(0));
    uint8_t rs1 = registerNameToNumber(operands.at(1));
    uint8_t rs2 = registerNameToNumber(operands.at(2));

    return encodeRType(0x0,rs2,rs1,0x0,rd,0x33);
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
