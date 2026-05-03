#pragma once

#include "Instruction.h"

namespace BType
{

struct InstructionArguments {
    const uint8_t rs1;
    const uint8_t rs2;
    const int32_t imm;
};

namespace InstructionNew
{
    constexpr uint8_t getInstructionDescription() { return 0x63; }

    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput);

    // Branch Equal
    namespace BEQ {
        constexpr uint8_t getInstructionDescriptor() { return 0x0; }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Branch Not Equal
    namespace BNE {
        constexpr uint8_t getInstructionDescriptor() { return 0x1; }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Branch Less Than
    namespace BLT {
        constexpr uint8_t getInstructionDescriptor() { return 0x4; }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Branch Greater or Equal
    namespace BGE {
        constexpr uint8_t getInstructionDescriptor() { return 0x5; }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Branch Less Than Unsigned
    namespace BLTU {
        constexpr uint8_t getInstructionDescriptor() { return 0x6; }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Branch Greater or Equal Unsigned
    namespace BGEU {
        constexpr uint8_t getInstructionDescriptor() { return 0x7; }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }
}

class Instruction : public ::Instruction
{
public:
    Instruction(uint32_t instruction) { this->instruction = instruction; }

    virtual ~Instruction() = default;
private:
    int32_t getImm();

protected:
    void decode() override;

    uint8_t rs1;
    uint8_t rs2;
    int32_t imm;
};

class InstructionFactory
{
public:
    static std::unique_ptr<Instruction> create(uint32_t encodedInstruction);

    static const uint8_t getInstructionDescription() { return 0x63; }
};

// Branch Equal
class BEQ : public Instruction
{
public:
    BEQ(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const uint8_t getInstructionDescriptor() { return 0x0; }
};

// Branch Not Equal
class BNE : public Instruction
{
public:
    BNE(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const uint8_t getInstructionDescriptor() { return 0x1; }
};

// Branch Less Than
class BLT : public Instruction
{
public:
    BLT(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const uint8_t getInstructionDescriptor() { return 0x4; }
};

// Branch Greater or Equal
class BGE : public Instruction
{
public:
    BGE(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const uint8_t getInstructionDescriptor() { return 0x5; }
};

// Branch Less Than Unsigned
class BLTU : public Instruction
{
public:
    BLTU(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const uint8_t getInstructionDescriptor() { return 0x6; }
};

// Branch Greater or Equal Unsigned
class BGEU : public Instruction
{
public:
    BGEU(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const uint8_t getInstructionDescriptor() { return 0x7; }
};

} // namespace BType
