#pragma once

#include "Instruction.h"

#include <memory>
#include <functional>

namespace IType
{

struct TupleHash
{
    std::size_t operator()(const std::tuple<uint8_t, uint8_t>& t) const
    {
        uint8_t first = std::get<0>(t);
        uint8_t second = std::get<1>(t);
        return (static_cast<std::size_t>(first) << 8) | second;
    }
};

class Instruction : public ::Instruction
{
public:
    Instruction(uint32_t instruction) { this->instruction = instruction; }

    virtual ~Instruction() = default;
protected:
    void decode() override;
    const int32_t getImm_i() const;
    const int8_t getShamt_i() const;

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

class SLLI : public Instruction
{
public:
    SLLI(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu) override;

                        // funct3, specialBit
    static const std::tuple<uint8_t, uint8_t> getInstructionDescriptor() { return {0x1, 0x0}; }
};

class SRLI : public Instruction
{
public:
    SRLI(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu) override;

                        // funct3, specialBit
    static const std::tuple<uint8_t, uint8_t> getInstructionDescriptor() { return {0x5, 0x0}; }
};

class SRAI : public Instruction
{
public:
    SRAI(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu) override;

                        // funct3, specialBit
    static const std::tuple<uint8_t, uint8_t> getInstructionDescriptor() { return {0x5, 0x1}; }
};

class LoadInstructionFactory
{
public:
    static std::unique_ptr<Instruction> create(uint32_t encodedInstruction);

    static const uint8_t getInstructionDescription() { return 0x3; }
};

class LB : public Instruction
{
public:
    LB(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu) override;

    static const uint8_t getInstructionDescriptor() { return 0x0; }
};

class LH : public Instruction
{
public:
    LH(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu) override;

    static const uint8_t getInstructionDescriptor() { return 0x1; }
};

class LW : public Instruction
{
public:
    LW(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu) override;

    static const uint8_t getInstructionDescriptor() { return 0x2; }
};

class LBU : public Instruction
{
public:
    LBU(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu) override;

    static const uint8_t getInstructionDescriptor() { return 0x4; }
};

class LHU : public Instruction
{
public:
    LHU(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu) override;

    static const uint8_t getInstructionDescriptor() { return 0x5; }
};

} // namespace IType
