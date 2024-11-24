#pragma once

#include "Instruction.h"

#include <memory>
#include <functional>

namespace RType
{
class Instruction : public ::Instruction
{
public:
    Instruction(uint32_t instruction) { this->instruction = instruction; }
    void execute(RiscvCpu& cpu) override;

protected:
    void decode() override;

    uint8_t rs1, rs2, rd;
    InstructionDescriptor descriptor;
};

class ADD : public Instruction
{
public:
    ADD(uint32_t instruction) : Instruction(instruction) {}
    void execute(RiscvCpu& cpu) override;

private:
    /// const uint8_t opcode = 0x33;
    /// const uint8_t funct3 = 0;
    /// const uint8_t funct7 = 0;
};

class InstructionFactory
{
public:
    static std::unique_ptr<Instruction> create(uint32_t encodedInstruction);
};

class InstructionDescriptor
{
public:
    InstructionDescriptor() = default;
    InstructionDescriptor(uint8_t opcode, uint8_t funct3, uint8_t funct7): opcode(opcode), funct3(funct3), funct7(funct7) {}

    // Getters
    uint8_t getOpcode() const { return opcode; }
    uint8_t getFunct3() const { return funct3; }
    uint8_t getFunct7() const { return funct7; }

    // Setters
    void setOpcode(uint8_t opcode) { this->opcode = opcode; }
    void setFunct3(uint8_t funct3) { this->funct3 = funct3; }
    void setFunct7(uint8_t funct7) { this->funct7 = funct7; }

    // Overloading
    bool operator==(const InstructionDescriptor& other) const { return this->opcode == other.opcode && this->funct3 == other.funct3 && this->funct7 == other.funct7; }

private:
    uint8_t opcode;
    uint8_t funct3;
    uint8_t funct7;
};

struct InstructionMapping
{
    InstructionDescriptor descriptor;

    std::function<std::unique_ptr<Instruction>(uint32_t)> create;
};

}
