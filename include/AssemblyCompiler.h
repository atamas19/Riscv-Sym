#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <bitset>
#include <sstream>

#include "instruction/Instruction.h"

class AssemblyInstruction;

class AssemblyCompiler
{
public:
    static uint32_t compile(const std::string& asmCode, InstructionOutput& instructionOutput);

    static AssemblyCompiler& getInstance();

private:
    AssemblyCompiler() = default;

private:
    uint32_t getInstruction(const std::string& instructionString);

    void setInstructionOutput(InstructionOutput& instructionOutput);

    uint32_t encodeRType(uint8_t funct7, uint8_t rs2, uint8_t rs1, uint8_t funct3, uint8_t rd, uint8_t opcode);

private:
    uint32_t assembleRType(const AssemblyInstruction& instruction, uint8_t funct3, uint8_t funct7);

    uint32_t assembleADD(const AssemblyInstruction& instruction);
    uint32_t assembleSUB(const AssemblyInstruction& instruction);
    uint32_t assembleSLL(const AssemblyInstruction& instruction);
    uint32_t assembleSLT(const AssemblyInstruction& instruction);
    uint32_t assembleSLTU(const AssemblyInstruction& instruction);
    uint32_t assembleXOR(const AssemblyInstruction& instruction);
    uint32_t assembleSRL(const AssemblyInstruction& instruction);
    uint32_t assembleSRA(const AssemblyInstruction& instruction);
    uint32_t assembleOR(const AssemblyInstruction& instruction);
    uint32_t assembleAND(const AssemblyInstruction& instruction);

private:
    InstructionOutput* instructionOutput;
};

class AssemblyInstruction
{
public:
    AssemblyInstruction(const std::string& line) { parse(line); }

    void parse(const std::string& line);

    void print() const;

    const std::string& getName() const;

    const std::vector<std::string>& getOperands() const;

private:
    void trim(std::string& s);

private:
    std::string instructionName;
    std::vector<std::string> operands;
};
