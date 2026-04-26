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

namespace Instruction_New {
    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr);
};

class Instruction
{
public:
    virtual void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) = 0;
    virtual ~Instruction() = default;
protected:
    virtual void decode() = 0;

    uint32_t instruction;
};

class InstructionFactory
{
public:
    static std::unique_ptr<Instruction> create(uint32_t encodedInstruction);
};

struct InstructionOutput
{
    int exitCode;
    std::string consoleLog;
    std::vector<uint8_t> modifiedRegisters;
    std::vector<MemoryCell> modifiedRamAddresses;

    void setRegisters(std::initializer_list<uint8_t> regs);
    void setRamAddresses(std::initializer_list<MemoryCell> memoryCell);
};
