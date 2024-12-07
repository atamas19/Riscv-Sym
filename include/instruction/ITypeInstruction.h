#pragma once

#include "Instruction.h"

#include <memory>
#include <functional>

namespace IType
{

class Instruction : public ::Instruction
{
public:
    Instruction(uint32_t instruction) { this->instruction = instruction; }

    virtual ~Instruction() = default;
protected:
    void decode() override;
    const int32_t getImm_i() const; 

    uint8_t rd;
    uint8_t rs1;
    int16_t imm;
};

class ArithmeticInstructionFactory
{
public:
    static std::unique_ptr<Instruction> create(uint32_t encodedInstruction);

    static const uint8_t getInstructionDescription() { return 0x13; }
};

class LoadInstructionFactory
{
public:
    static std::unique_ptr<Instruction> create(uint32_t encodedInstruction);

    static const uint8_t getInstructionDescription() { return 0x3; }
};

class ADDI : public Instruction
{
public:
    ADDI(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu) override;

    static const uint8_t getInstructionDescriptor() { return 0x0; }
};

class SLTI : public Instruction
{
public:
    SLTI(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu) override;

    static const uint8_t getInstructionDescriptor() { return 0x2; }
};

class SLTIU : public Instruction
{
public:
    SLTIU(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu) override;

    static const uint8_t getInstructionDescriptor() { return 0x3; }
};

class XORI : public Instruction
{
public:
    XORI(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu) override;

    static const uint8_t getInstructionDescriptor() { return 0x4; }
};

class ORI : public Instruction
{
public:
    ORI(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu) override;

    static const uint8_t getInstructionDescriptor() { return 0x6; }
};

class ANDI : public Instruction
{
public:
    ANDI(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu) override;

    static const uint8_t getInstructionDescriptor() { return 0x7; }
};

} // namespace IType
