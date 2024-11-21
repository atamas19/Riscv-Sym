#pragma once

#include "Instruction.h"

#include <memory>

class RTypeInstruction : public Instruction
{
public:
    RTypeInstruction(uint32_t instruction) { this->instruction = instruction; }
    void execute(RiscvCpu& cpu) override;

protected:
    void decode() override;

    uint8_t rs1, rs2, rd;
    uint8_t opcode, funct3, funct7;
};

class ADD : public RTypeInstruction
{
public:
    void execute(RiscvCpu& cpu) override;

private:
    /// const uint8_t opcode = 0x33;
    /// const uint8_t funct3 = 0;
    /// const uint8_t funct7 = 0;
};

class RTypeInstructionFactory
{
public:
    static std::unique_ptr<Instruction> create(uint32_t encodedInstruction);
};
