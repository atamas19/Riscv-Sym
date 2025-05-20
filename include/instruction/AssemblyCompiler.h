#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <bitset>
#include <sstream>

#include "Instruction.h"

class AssemblyCompiler
{
public:
    static uint32_t compile(const std::string& asmCode);

    static AssemblyCompiler& getInstance();

private:
    uint32_t getInstruction(std::istringstream& instruction);

    uint32_t encodeRType(uint8_t funct7, uint8_t rs2, uint8_t rs1, uint8_t funct3, uint8_t rd, uint8_t opcode);

private:
    uint32_t assembleAdd(std::istringstream& instruction);
};
