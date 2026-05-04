#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <bitset>
#include <initializer_list>
#include <string>

#include <core/Memory.h>

class RiscvCpu;
struct InstructionOutput;

inline uint32_t getBits(uint32_t instruction, uint8_t x, uint8_t y) {
    if (x > y || y >= 32)
        throw std::out_of_range("Invalid bit range");

    uint32_t mask = (1ULL << (y - x + 1)) - 1;
    return (instruction >> x) & mask;
}

constexpr uint16_t createInstructionDescription(uint8_t funct3, uint8_t funct7) {
    return (static_cast<uint16_t>(funct3) << 8) | funct7;
}

uint16_t createRuntimeInstructionDescription(uint8_t funct3, uint8_t funct7);

namespace Instruction
{
    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr);
} // namespace Instruction

struct InstructionOutput
{
    int exitCode;
    std::string consoleLog;
    std::vector<uint8_t> modifiedRegisters;
    std::vector<MemoryCell> modifiedRamAddresses;

    void setRegisters(std::initializer_list<uint8_t> regs);
    void setRamAddresses(std::initializer_list<MemoryCell> memoryCell);
};
