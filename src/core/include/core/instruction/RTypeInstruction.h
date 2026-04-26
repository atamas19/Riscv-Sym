#pragma once

#include "Instruction.h"

#include <memory>
#include <functional>
#include <algorithm>

namespace RType
{

constexpr uint16_t createInstructionDescription(uint8_t funct3, uint8_t funct7) {
    return (static_cast<uint16_t>(funct3) << 8) | funct7;
}

struct InstructionArguments {
    uint8_t rs1;
    uint8_t rs2;
    uint8_t rd;
};

namespace InstructionNew
{
    constexpr uint8_t getInstructionDescription() { return 0x33; }

    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput);

    // Add
    namespace ADD {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x0, 0x0); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    };

    // Substract
    namespace SUB {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x0, 0x20); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    };

    // Shift Left Logical
    namespace SLL {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x1, 0x0); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    }

    // Set Less Than
    namespace SLT {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x2, 0x0); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    };

    // Set Less Than Unsigned
    namespace SLTU {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x3, 0x0); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    };

    // Exclusive Or
    namespace XOR {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x4, 0x0); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    };

    // Shit Right Logical
    namespace SRL {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x5, 0x0); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    };

    // Shift Right Arithmetic
    namespace SRA {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x5, 0x20); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    };

    // Or
    namespace OR {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x6, 0x0); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    };

    // And
    namespace AND {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x7, 0x0); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    };

    // RV32M instructions

    // Multiply
    namespace MUL {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x0, 0x1); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    };

    // Multiply High
    namespace MULH {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x1, 0x1); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    };

    // Multiply High Signed-Unsigned
    namespace MULHSU {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x2, 0x1); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    };

    // Multiply High Unsigned
    namespace MULHU {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x3, 0x1); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    };

    // Divide
    namespace DIV {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x4, 0x1); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    };

    // Divide Unsigned
    namespace DIVU {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x5, 0x1); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    };

    // Remainder
    namespace REM {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x6, 0x1); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    };

    // Remainder Unsigned
    namespace REMU {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x7, 0x1); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    };

};

// RV32A instructions
namespace AtomicNew
{
    constexpr uint8_t getInstructionDescription() { return 0x2F; }

    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput);

    namespace LR {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x2, 0x2); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    };

    namespace SC {
        constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x2, 0x3); }

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
    };

    namespace AMO
    {
        struct InstructionArguments : public RType::InstructionArguments {
            std::string instructionName;
            std::function<uint32_t(uint32_t, uint32_t)> performAmoOperation;

            InstructionArguments(RType::InstructionArguments base,
                                std::string name,
                                std::function<uint32_t(uint32_t, uint32_t)> op)
                : RType::InstructionArguments(base),
                instructionName(std::move(name)),
                performAmoOperation(std::move(op))
            {}
        };

        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);

        namespace SWAP {
            constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x2, 0x1); }

            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
        };

        namespace ADD {
            constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x2, 0x0); }

            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
        };

        namespace XOR {
            constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x2, 0x4); }

            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
        };

        namespace AND {
            constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x2, 0xC); }

            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
        };

        namespace OR {
            constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x2, 0x8); }

            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
        };

        namespace MIN {
            constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x2, 0x10); }

            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
        };

        namespace MAX {
            constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x2, 0x14); }

            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
        };

        namespace MINU {
            constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x2, 0x18); }

            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
        };

        namespace MAXU {
            constexpr uint16_t getInstructionDescription() { return createInstructionDescription(0x2, 0x1C); }

            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput);
        };
    };
};

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
    uint8_t funct3;
    uint8_t funct7;
};

class Instruction : public ::Instruction
{
public:
    Instruction(uint32_t instruction) { this->instruction = instruction; }

    virtual ~Instruction() = default;
protected:
    void decode() override;

    uint8_t rs1, rs2, rd;
};

class InstructionFactory
{
public:
    static std::unique_ptr<Instruction> create(uint32_t encodedInstruction);

    static constexpr uint8_t getInstructionDescription() { return 0x33; }
};

// Add
class ADD : public Instruction
{
public:
    ADD(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x0, 0x0}; }
};

// Substract
class SUB : public Instruction
{
public:
    SUB(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x0, 0x20}; }
};

// Shift Left Logical
class SLL : public Instruction
{
public:
    SLL(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x1, 0x0}; }
};

// Set Less Than
class SLT : public Instruction
{
public:
    SLT(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x2, 0x0}; }
};

// Set Less Than Unsigned
class SLTU : public Instruction
{
public:
    SLTU(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x3, 0x0}; }
};

// Exclusive Or
class XOR : public Instruction
{
public:
    XOR(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x4, 0x0}; }
};

// Shit Right Logical
class SRL : public Instruction
{
public:
    SRL(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x5, 0x0}; }
};

// Shift Right Arithmetic
class SRA : public Instruction
{
public:
    SRA(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x5, 0x20}; }
};

// Or
class OR : public Instruction
{
public:
    OR(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x6, 0x0}; }
};

// And
class AND : public Instruction
{
public:
    AND(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x7, 0x0}; }
};

// RV32M instructions

// Multiply
class MUL : public Instruction
{
public:
    MUL(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x0, 0x1}; }
};

// Multiply High
class MULH : public Instruction
{
public:
    MULH(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x1, 0x1}; }
};

// Multiply High Signed-Unsigned
class MULHSU : public Instruction
{
public:
    MULHSU(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x2, 0x1}; }
};

// Multiply High Unsigned
class MULHU : public Instruction
{
public:
    MULHU(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x3, 0x1}; }
};

// Divide
class DIV : public Instruction
{
public:
    DIV(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x4, 0x1}; }
};

// Divide Unsigned
class DIVU : public Instruction
{
public:
    DIVU(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x5, 0x1}; }
};

// Remainder
class REM : public Instruction
{
public:
    REM(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x6, 0x1}; }
};

// Remainder Unsigned
class REMU : public Instruction
{
public:
    REMU(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x7, 0x1}; }
};

// RV32A Instructions

class AtomicInstructionFactory
{
public:
    static std::unique_ptr<Instruction> create(uint32_t encodedInstruction);

    static constexpr uint8_t getInstructionDescription() { return 0x2F; }
};

// Load Reserved
class LR : public Instruction {
public:
    LR(uint32_t ins) : Instruction(ins) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x2, 0x2}; }
};

// Store Conditional
class SC : public Instruction {
public:
    SC(uint32_t ins) : Instruction(ins) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;

    static const InstructionDescriptor getInstructionDescriptor() { return {0x2, 0x3}; }
};

// Atomic Memory Operation
namespace AMO
{

class Instruction : public RType::Instruction {
public:
    Instruction(uint32_t instruction) : RType::Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput* instructionOutput = nullptr) override;
protected:
    virtual uint32_t performAmoOperation(uint32_t oldValue, uint32_t rs2Value) const = 0;
    virtual std::string getInstructionName() const = 0;
};

// Atomic Memory Operation Swap
class SWAP : public Instruction {
public:
    SWAP(uint32_t ins) : Instruction(ins) {}
    static const InstructionDescriptor getInstructionDescriptor() { return {0x2, 0x1}; }
protected:
    uint32_t performAmoOperation(uint32_t oldValue, uint32_t rs2Value) const override { return rs2Value; }
    std::string getInstructionName() const override { return "AMOSWAP"; }
};

// Atomic Memory Operation Add
class ADD : public Instruction {
public:
    ADD(uint32_t ins) : Instruction(ins) {}
    static const InstructionDescriptor getInstructionDescriptor() { return {0x2, 0x0}; }
protected:
    uint32_t performAmoOperation(uint32_t oldValue, uint32_t rs2Value) const override { return oldValue + rs2Value; }
    std::string getInstructionName() const override { return "AMOADD"; }
};

// Atomic Memory Operation XOR
class XOR : public Instruction {
public:
    XOR(uint32_t ins) : Instruction(ins) {}
    static const InstructionDescriptor getInstructionDescriptor() { return {0x2, 0x4}; }
protected:
    uint32_t performAmoOperation(uint32_t oldValue, uint32_t rs2Value) const override { return oldValue ^ rs2Value; }
    std::string getInstructionName() const override { return "AMOXOR"; }
};

// Atomic Memory Operation AND
class AND : public Instruction {
public:
    AND(uint32_t ins) : Instruction(ins) {}
    static const InstructionDescriptor getInstructionDescriptor() { return {0x2, 0xC}; }
protected:
    uint32_t performAmoOperation(uint32_t oldValue, uint32_t rs2Value) const override { return oldValue & rs2Value; }
    std::string getInstructionName() const override { return "AMOAND"; }
};

// Atomic Memory Operation OR
class OR : public Instruction {
public:
    OR(uint32_t ins) : Instruction(ins) {}
    static const InstructionDescriptor getInstructionDescriptor() { return {0x2, 0x8}; }
protected:
    uint32_t performAmoOperation(uint32_t oldValue, uint32_t rs2Value) const override { return oldValue | rs2Value; }
    std::string getInstructionName() const override { return "AMOOR"; }
};

// Atomic Memory Operation Min
class MIN : public Instruction {
public:
    MIN(uint32_t ins) : Instruction(ins) {}
    static const InstructionDescriptor getInstructionDescriptor() { return {0x2, 0x10}; }
protected:
    uint32_t performAmoOperation(uint32_t oldValue, uint32_t rs2Value) const override {
        int32_t v1 = static_cast<int32_t>(oldValue);
        int32_t v2 = static_cast<int32_t>(rs2Value);
        return static_cast<uint32_t>(std::min(v1, v2));
    }
    std::string getInstructionName() const override { return "AMOMIN"; }
};

// Atomic Memory Operation Max
class MAX : public Instruction {
public:
    MAX(uint32_t ins) : Instruction(ins) {}
    static const InstructionDescriptor getInstructionDescriptor() { return {0x2, 0x14}; }
protected:
    uint32_t performAmoOperation(uint32_t oldValue, uint32_t rs2Value) const override {
        int32_t v1 = static_cast<int32_t>(oldValue);
        int32_t v2 = static_cast<int32_t>(rs2Value);
        return static_cast<uint32_t>(std::max(v1, v2));
    }
    std::string getInstructionName() const override { return "AMOMAX"; }
};

// Atomic Memory Operation Min Unsigned
class MINU : public Instruction {
public:
    MINU(uint32_t ins) : Instruction(ins) {}
    static const InstructionDescriptor getInstructionDescriptor() { return {0x2, 0x18}; }
protected:
    uint32_t performAmoOperation(uint32_t oldValue, uint32_t rs2Value) const override { return std::min(oldValue, rs2Value); }
    std::string getInstructionName() const override { return "AMOMINU"; }
};

// Atomic Memory Operation Max Unsigned
class MAXU : public Instruction {
public:
    MAXU(uint32_t ins) : Instruction(ins) {}
    static const InstructionDescriptor getInstructionDescriptor() { return {0x2, 0x1C}; }
protected:
    uint32_t performAmoOperation(uint32_t oldValue, uint32_t rs2Value) const override { return std::max(oldValue, rs2Value); }
    std::string getInstructionName() const override { return "AMOMAXU"; }
};

} // namespace AMO


} // namespace RType
