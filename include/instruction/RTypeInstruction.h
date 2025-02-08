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
    InstructionDescriptor(uint8_t funct3, uint8_t funct7): funct3(funct3), funct7(funct7) {}

    struct InstructionDescriptorHash
    {
        std::size_t operator()(const InstructionDescriptor& desc) const
        {
            return (desc.getFunct3() << 8) | desc.getFunct7();
        }
    };

    // Getters
    uint8_t getOpcode() const { return opcode; }
    uint8_t getFunct3() const { return funct3; }
    uint8_t getFunct7() const { return funct7; }

    // Setters
    void setFunct3(uint8_t funct3) { this->funct3 = funct3; }
    void setFunct7(uint8_t funct7) { this->funct7 = funct7; }

    // Operator overloading
    bool operator==(const InstructionDescriptor& other) const
    {
        return this->funct3 == other.funct3 && this->funct7 == other.funct7;
    }

private:
    const uint8_t opcode = 0x33;
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

class InstructionFactory
{
public:
    static std::unique_ptr<Instruction> create(uint32_t encodedInstruction);

    static const uint8_t getInstructionDescription() { return 0x33; }
};

// Add
class ADD : public Instruction
{
public:
    ADD(uint32_t instruction, InstructionDescriptor descriptor) : Instruction(instruction, descriptor) { decode(); }
    void execute(RiscvCpu& cpu) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x0, 0x0}; }
};

// Substract
class SUB : public Instruction
{
public:
    SUB(uint32_t instruction, InstructionDescriptor descriptor) : Instruction(instruction, descriptor) { decode(); }
    void execute(RiscvCpu& cpu) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x0, 0x20}; }
};

// Shift Left Logical
class SLL : public Instruction
{
public:
    SLL(uint32_t instruction, InstructionDescriptor descriptor) : Instruction(instruction, descriptor) { decode(); }
    void execute(RiscvCpu& cpu) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x1, 0x0}; }
};

// Set Less Than
class SLT : public Instruction
{
public:
    SLT(uint32_t instruction, InstructionDescriptor descriptor) : Instruction(instruction, descriptor) { decode(); }
    void execute(RiscvCpu& cpu) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x2, 0x0}; }
};

// Set Less Than Unsigned
class SLTU : public Instruction
{
public:
    SLTU(uint32_t instruction, InstructionDescriptor descriptor) : Instruction(instruction, descriptor) { decode(); }
    void execute(RiscvCpu& cpu) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x3, 0x0}; }
};

// Exclusive Or
class XOR : public Instruction
{
public:
    XOR(uint32_t instruction, InstructionDescriptor descriptor) : Instruction(instruction, descriptor) { decode(); }
    void execute(RiscvCpu& cpu) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x4, 0x0}; }
};

// Shit Right Logical
class SRL : public Instruction
{
public:
    SRL(uint32_t instruction, InstructionDescriptor descriptor) : Instruction(instruction, descriptor) { decode(); }
    void execute(RiscvCpu& cpu) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x5, 0x0}; }
};

// Shift Right Arithmetic
class SRA : public Instruction
{
public:
    SRA(uint32_t instruction, InstructionDescriptor descriptor) : Instruction(instruction, descriptor) { decode(); }
    void execute(RiscvCpu& cpu) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x5, 0x20}; }
};

// Or
class OR : public Instruction
{
public:
    OR(uint32_t instruction, InstructionDescriptor descriptor) : Instruction(instruction, descriptor) { decode(); }
    void execute(RiscvCpu& cpu) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x6, 0x0}; }
};

// And
class AND : public Instruction
{
public:
    AND(uint32_t instruction, InstructionDescriptor descriptor) : Instruction(instruction, descriptor) { decode(); }
    void execute(RiscvCpu& cpu) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x7, 0x0}; }
};

} // namespace RType
