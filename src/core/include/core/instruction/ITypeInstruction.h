#pragma once

#include "Instruction.h"

#include <functional>

namespace IType
{

struct InstructionArguments {
    const uint8_t rs1;
    const uint8_t rd;
    const int32_t imm;
};

namespace ArithmeticInstruction
{
    constexpr uint8_t getInstructionDescription() { return 0x13; }

    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput);

    // Add Immediate
    namespace ADDI {
        constexpr uint8_t getInstructionDescription() { return 0x0; }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Set Less Than Immediate
    namespace SLTI {
        constexpr uint8_t getInstructionDescription() { return 0x2; }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Set Less Than Immediate Unsigned
    namespace SLTIU {
        constexpr uint8_t getInstructionDescription() { return 0x3; }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Exclusive Or Immediate
    namespace XORI {
        constexpr uint8_t getInstructionDescription() { return 0x4; }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Or Immediate
    namespace ORI {
        constexpr uint8_t getInstructionDescription() { return 0x6; }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // And Immediate
    namespace ANDI {
        constexpr uint8_t getInstructionDescription() { return 0x7; }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Shift Left Logical Immediate
    namespace SLLI {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x1, 0x0); }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Shit Right Logical Immediate
    namespace SRLI {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x5, 0x0); }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Shift Right Arithmetic Immediate
    namespace SRAI {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x5, 0x1); }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }
}

namespace LoadInstruction
{
    constexpr uint8_t getInstructionDescription() { return 0x3; }

    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput);

    // Load Byte
    namespace LB {
        constexpr uint8_t getInstructionDescription() { return 0x0; }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Load Halfword
    namespace LH {
        constexpr uint8_t getInstructionDescription() { return 0x1; }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Load Word
    namespace LW {
        constexpr uint8_t getInstructionDescription() { return 0x2; }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Load Byte Unsigned
    namespace LBU {
        constexpr uint8_t getInstructionDescription() { return 0x4; }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Load Halfword Unsigned
    namespace LHU {
        constexpr uint8_t getInstructionDescription() { return 0x5; }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }
}

// Special case for JALR
// Jump And Link Register
namespace JALR_NEW {
    constexpr uint8_t getInstructionDescription() { return 0x67; }

    bool execute(const uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput);
}

namespace FenceInstruction
{
    constexpr uint8_t getInstructionDescription() { return 0x0F; }

    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput);

    namespace FENCE {
        constexpr uint8_t getInstructionDescription() { return 0x0; }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    namespace FENCE_I {
        constexpr uint8_t getInstructionDescription() { return 0x1; }

        bool execute(const InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

}

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

    uint8_t rd;
    uint8_t rs1;
    int32_t imm;
};

class ArithmeticInstructionFactory
{
public:
    static std::unique_ptr<Instruction> create(uint32_t encodedInstruction);

    static const uint8_t getInstructionDescription() { return 0x13; }
};

// Add Immediate
class ADDI : public Instruction
{
public:
    ADDI(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const uint8_t getInstructionDescriptor() { return 0x0; }
};

// Set Less Than Immediate
class SLTI : public Instruction
{
public:
    SLTI(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const uint8_t getInstructionDescriptor() { return 0x2; }
};

// Set Less Than Immediate Unsigned
class SLTIU : public Instruction
{
public:
    SLTIU(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const uint8_t getInstructionDescriptor() { return 0x3; }
};

// Exclusive Or Immediate
class XORI : public Instruction
{
public:
    XORI(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const uint8_t getInstructionDescriptor() { return 0x4; }
};

// Or Immediate
class ORI : public Instruction
{
public:
    ORI(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const uint8_t getInstructionDescriptor() { return 0x6; }
};

// And Immediate
class ANDI : public Instruction
{
public:
    ANDI(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const uint8_t getInstructionDescriptor() { return 0x7; }
};

// Shift Left Logical Immediate
class SLLI : public Instruction
{
public:
    SLLI(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

                        // funct3, specialBit
    static const std::tuple<uint8_t, uint8_t> getInstructionDescriptor() { return {0x1, 0x0}; }
};

// Shit Right Logical Immediate
class SRLI : public Instruction
{
public:
    SRLI(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

                        // funct3, specialBit
    static const std::tuple<uint8_t, uint8_t> getInstructionDescriptor() { return {0x5, 0x0}; }
};

// Shift Right Arithmetic Immediate
class SRAI : public Instruction
{
public:
    SRAI(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

                        // funct3, specialBit
    static const std::tuple<uint8_t, uint8_t> getInstructionDescriptor() { return {0x5, 0x1}; }
};

class LoadInstructionFactory
{
public:
    static std::unique_ptr<Instruction> create(uint32_t encodedInstruction);

    static const uint8_t getInstructionDescription() { return 0x3; }
};

// Load Byte
class LB : public Instruction
{
public:
    LB(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const uint8_t getInstructionDescriptor() { return 0x0; }
};

// Load Halfword
class LH : public Instruction
{
public:
    LH(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const uint8_t getInstructionDescriptor() { return 0x1; }
};

// Load Word
class LW : public Instruction
{
public:
    LW(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const uint8_t getInstructionDescriptor() { return 0x2; }
};

// Load Byte Unsigned
class LBU : public Instruction
{
public:
    LBU(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const uint8_t getInstructionDescriptor() { return 0x4; }
};

// Load Halfword Unsigned
class LHU : public Instruction
{
public:
    LHU(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const uint8_t getInstructionDescriptor() { return 0x5; }
};

// Special case for JALR
// Jump And Link Register
class JALR : public Instruction
{
public:
    JALR(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const uint8_t getInstructionDescriptor() { return 0x67; }
};

class FenceInstructionFactory
{
public:
    static std::unique_ptr<Instruction> create(uint32_t encodedInstruction);

    static constexpr uint8_t getInstructionDescription() { return 0x0F; }
};

class FENCE : public Instruction {
public:
    FENCE(uint32_t instruction) : IType::Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static constexpr uint8_t getInstructionDescriptor() { return 0x0; }
};

class FENCE_I : public Instruction {
public:
    FENCE_I(uint32_t instruction) : IType::Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static constexpr uint8_t getInstructionDescriptor() { return 0x1; }
};

} // namespace IType
