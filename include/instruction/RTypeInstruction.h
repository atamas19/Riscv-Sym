#pragma once

#include "Instruction.h"

#include <memory>
#include <functional>

namespace RType
{

class InstructionDescriptor
{
public:
    InstructionDescriptor() = default;
    InstructionDescriptor(uint8_t opcode, uint8_t funct3, uint8_t funct7): opcode(opcode), funct3(funct3), funct7(funct7) {}

    struct InstructionDescriptorHash
    {
        std::size_t operator()(const InstructionDescriptor& desc) const
        {
            return (desc.getOpcode() << 16) | (desc.getFunct3() << 8) | desc.getFunct7();
        }
    };

    // Getters
    uint8_t getOpcode() const { return opcode; }
    uint8_t getFunct3() const { return funct3; }
    uint8_t getFunct7() const { return funct7; }

    // Setters
    void setOpcode(uint8_t opcode) { this->opcode = opcode; }
    void setFunct3(uint8_t funct3) { this->funct3 = funct3; }
    void setFunct7(uint8_t funct7) { this->funct7 = funct7; }

    // Operator overloading
    bool operator==(const InstructionDescriptor& other) const
    {
        return this->opcode == other.opcode &&
               this->funct3 == other.funct3 &&
               this->funct7 == other.funct7;
    }

private:
    uint8_t opcode;
    uint8_t funct3;
    uint8_t funct7;
};

class Instruction : public ::Instruction
{
public:
    Instruction(uint32_t instruction, InstructionDescriptor descriptor) : descriptor(descriptor) { this->instruction = instruction; }

    virtual ~Instruction() = default;
protected:
    void decode() override;

    uint8_t rs1, rs2, rd;
    InstructionDescriptor descriptor;
};

class ADD : public Instruction
{
public:
    ADD(uint32_t instruction, InstructionDescriptor descriptor) : Instruction(instruction, descriptor) { decode(); }
    void execute(RiscvCpu& cpu) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x33, 0x0, 0x0}; }
};

class SUB : public Instruction
{
public:
    SUB(uint32_t instruction, InstructionDescriptor descriptor) : Instruction(instruction, descriptor) { decode(); }
    void execute(RiscvCpu& cpu) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x33, 0x0, 0x20}; }
};

class SLL : public Instruction
{
public:
    SLL(uint32_t instruction, InstructionDescriptor descriptor) : Instruction(instruction, descriptor) { decode(); }
    void execute(RiscvCpu& cpu) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x33, 0x1, 0x0}; }
};

class InstructionFactory
{
public:
    static std::unique_ptr<Instruction> create(uint32_t encodedInstruction);
};

} // namespace RType
