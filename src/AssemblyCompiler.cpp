#include "AssemblyCompiler.h"

#include <unordered_map>
#include <cassert>
#include <functional>
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

bool AssemblyCompiler::validateRegister(const std::optional<uint8_t>& reg, const std::string& name)
{
    if (!reg)
    {
        instructionOutput->consoleLog = name + " is not a valid register.";
        instructionOutput->exitCode = -1;
        return false;
    }
    return true;
};

uint32_t AssemblyCompiler::compile(const std::string& asmCode, InstructionOutput& instructionOutput)
{
    auto& assemblyCompiler = AssemblyCompiler::getInstance();

    assemblyCompiler.setInstructionOutput(instructionOutput);

    uint32_t encodedInstruction = assemblyCompiler.getInstruction(asmCode);

    return encodedInstruction;
}

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
        {"and",  [this](const AssemblyInstruction& ins) { return assembleAND(ins);  }},
        // BType instructions
        {"beq",  [this](const AssemblyInstruction& ins) { return assembleBEQ(ins);  }},
        {"bne",  [this](const AssemblyInstruction& ins) { return assembleBNE(ins);  }},
        {"blt",  [this](const AssemblyInstruction& ins) { return assembleBLT(ins);  }},
        {"bge",  [this](const AssemblyInstruction& ins) { return assembleBGE(ins);  }},
        {"bltu", [this](const AssemblyInstruction& ins) { return assembleBLTU(ins); }},
        {"bgeu", [this](const AssemblyInstruction& ins) { return assembleBGEU(ins); }},
        // JType instructions
        {"jal",  [this](const AssemblyInstruction& ins) { return assembleJAL(ins);  }},
        // UType instructions
        {"lui",  [this](const AssemblyInstruction& ins) { return assembleLUI(ins);  }},
        {"auipc",[this](const AssemblyInstruction& ins) { return assembleAUIPC(ins);}}

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

uint32_t AssemblyCompiler::encodeBType(int16_t imm, uint8_t rs2, uint8_t rs1, uint8_t funct3, uint8_t opcode)
{
    uint32_t encoded = 0;
    uint16_t imm12 = static_cast<uint16_t>(imm);

    encoded |= ((imm12 >> 12) & 0x1)     << 31;
    encoded |= ((imm12 >> 5)  & 0x3F)    << 25;
    encoded |= (rs2 & 0x1F)              << 20;
    encoded |= (rs1 & 0x1F)              << 15;
    encoded |= (funct3 & 0x07)           << 12;
    encoded |= ((imm12 >> 1) & 0x0F)     << 8;
    encoded |= ((imm12 >> 11) & 0x1)     << 7;
    encoded |= (opcode & 0x7F);

    return encoded;
}

uint32_t AssemblyCompiler::assembleBType(const AssemblyInstruction& instruction, uint8_t funct3)
{
    auto& operands = instruction.getOperands();
    if (operands.size() != 3)
    {
        instructionOutput->consoleLog = "Not the right number of operands, should be exactly 3!";
        instructionOutput->exitCode = -1;
        return 0;
    }

    auto rs1 = registerNameToNumber(operands[0]);
    auto rs2 = registerNameToNumber(operands[1]);

    if (!validateRegister(rs1, operands.at(1)) || !validateRegister(rs2, operands.at(2)))
        return 0;

    int imm;
    try {
        imm = std::stoi(operands[2]);
    } catch (...) {
        instructionOutput->consoleLog = "Invalid immediate value!";
        instructionOutput->exitCode = -1;
        return 0;
    }

    if (imm % 4 != 0)
    {
        instructionOutput->consoleLog = "Branch offset must be a multiple of 4!";
        instructionOutput->exitCode = -1;
        return 0;
    }

    if (imm < -4096 || imm > 4094)
    {
        instructionOutput->consoleLog = "Branch offset out of 13-bit signed range (-4096 to 4094)!";
        instructionOutput->exitCode = -1;
        return 0;
    }

    return encodeBType(static_cast<int16_t>(imm), *rs2, *rs1, funct3, 0x63);
}

uint32_t AssemblyCompiler::assembleBEQ(const AssemblyInstruction& instruction)
{
    uint8_t funct3 = 0x0;
    return assembleBType(instruction, funct3);
}

uint32_t AssemblyCompiler::assembleBNE(const AssemblyInstruction& instruction)
{
    uint8_t funct3 = 0x1;
    return assembleBType(instruction, funct3);
}

uint32_t AssemblyCompiler::assembleBLT(const AssemblyInstruction& instruction)
{
    uint8_t funct3 = 0x4;
    return assembleBType(instruction, funct3);
}

uint32_t AssemblyCompiler::assembleBGE(const AssemblyInstruction& instruction)
{
    uint8_t funct3 = 0x5;
    return assembleBType(instruction, funct3);
}

uint32_t AssemblyCompiler::assembleBLTU(const AssemblyInstruction& instruction)
{
    uint8_t funct3 = 0x6;
    return assembleBType(instruction, funct3);
}

uint32_t AssemblyCompiler::assembleBGEU(const AssemblyInstruction& instruction)
{
    uint8_t funct3 = 0x7;
    return assembleBType(instruction, funct3);
}

uint32_t AssemblyCompiler::encodeJType(int32_t imm, uint8_t rd, uint8_t opcode)
{
    uint32_t encoded = 0;
    uint32_t imm20 = static_cast<uint32_t>(imm);

    encoded |= ((imm20 >> 20) & 0x1)     << 31;
    encoded |= ((imm20 >> 1)  & 0x3FF)   << 21;
    encoded |= ((imm20 >> 11) & 0x1)     << 20;
    encoded |= ((imm20 >> 12) & 0xFF)    << 12;
    encoded |= (rd & 0x1F)               << 7;
    encoded |= (opcode & 0x7F);

    return encoded;
}

uint32_t AssemblyCompiler::assembleJType(const AssemblyInstruction& instruction)
{
    auto& operands = instruction.getOperands();
    if (operands.size() != 2)
    {
        instructionOutput->consoleLog = "Not the right number of operands, should be exactly 2!";
        instructionOutput->exitCode = -1;
        return 0;
    }

    auto rd = registerNameToNumber(operands[0]);

    if (!validateRegister(rd, operands.at(0)))
        return 0;

    int imm;
    try {
        imm = std::stoi(operands[1]);
    } catch (...) {
        instructionOutput->consoleLog = "Invalid immediate value!";
        instructionOutput->exitCode = -1;
        return 0;
    }

    if (imm % 4 != 0)
    {
        instructionOutput->consoleLog = "Jump offset must be a multiple of 4!";
        instructionOutput->exitCode = -1;
        return 0;
    }

    if (imm < -1048576 || imm > 1048574)
    {
        instructionOutput->consoleLog = "Jump offset out of 21-bit signed range (-1048576 to 1048574)!";
        instructionOutput->exitCode = -1;
        return 0;
    }

    return encodeJType(static_cast<int32_t>(imm), *rd, 0x6F);
}

uint32_t AssemblyCompiler::assembleJAL(const AssemblyInstruction& instruction)
{
    return assembleJType(instruction);
}

uint32_t AssemblyCompiler::encodeUType(uint32_t imm, uint8_t rd, uint8_t opcode)
{
    uint32_t encoded = 0;
    encoded |= (imm & 0xFFFFF000);
    encoded |= (rd & 0x1F) << 7;
    encoded |= (opcode & 0x7F);

    return encoded;
}

uint32_t AssemblyCompiler::assembleUType(const AssemblyInstruction& instruction, uint8_t opcode)
{
    const auto& operands = instruction.getOperands();
    if (operands.size() != 2)
    {
        instructionOutput->consoleLog = "U-type instruction requires exactly 2 operands.";
        instructionOutput->exitCode = -1;
        return 0;
    }

    auto rd = registerNameToNumber(operands[0]);
    if (!validateRegister(rd, operands[0]))
        return 0;

    uint32_t imm;
    try {
        imm = std::stoul(operands[1], nullptr, 0);
    } catch (...) {
        instructionOutput->consoleLog = "Invalid immediate value.";
        instructionOutput->exitCode = -1;
        return 0;
    }

    if (imm > 0xFFFFF)
        imm &= 0xFFFFF;

    imm = (imm << 12); // Shifts by 12 bits so the lower 3 bytes are 0

    return encodeUType(imm, *rd, opcode);
}

uint32_t AssemblyCompiler::assembleLUI(const AssemblyInstruction& instruction)
{
    return assembleUType(instruction, 0x37);
}

uint32_t AssemblyCompiler::assembleAUIPC(const AssemblyInstruction& instruction)
{
    return assembleUType(instruction, 0x17);
}
