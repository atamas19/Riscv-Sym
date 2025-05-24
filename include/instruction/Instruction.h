#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <bitset>
#include <initializer_list>
#include <string>

class RiscvCpu;
struct InstructionOutput;

uint32_t getBits(uint32_t instruction, uint8_t x, uint8_t y);

class Instruction
{
public:
    virtual void execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) = 0;
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
    std::vector<uint32_t> modifiedRamAddresses;

    void setRegisters(std::initializer_list<uint8_t> regs);
    void setRamAddresses(std::initializer_list<uint32_t> addresses);
};
