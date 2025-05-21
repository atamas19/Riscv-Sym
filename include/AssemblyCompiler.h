#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <bitset>
#include <sstream>

class AssemblyInstruction;

class AssemblyCompiler
{
public:
    static uint32_t compile(const std::string& asmCode);

    static AssemblyCompiler& getInstance();

private:
    uint32_t getInstruction(const std::string& instructionString);

    uint32_t encodeRType(uint8_t funct7, uint8_t rs2, uint8_t rs1, uint8_t funct3, uint8_t rd, uint8_t opcode);

private:
    uint32_t assembleAdd(const AssemblyInstruction& instruction);
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
