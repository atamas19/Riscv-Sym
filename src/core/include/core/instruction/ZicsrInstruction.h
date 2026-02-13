#pragma once

#include "ITypeInstruction.h"


namespace Zicsr {

class Instruction : public IType::Instruction {
public:
    Instruction(uint32_t instruction) : IType::Instruction(instruction) {}

    virtual ~Instruction() = default;
protected:
    void decode() override;

    uint16_t csr_addr;
};

class InstructionFactory {
public:
    static std::unique_ptr<Instruction> create(uint32_t encodedInstruction);

    static constexpr uint8_t getInstructionDescription() { return 0x73; }
};

// CSR Read/Write
class CSRRW final : public Instruction {
public:
    CSRRW(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) override;

    static constexpr uint8_t getInstructionDescriptor() { return 0x1; }
};

// CSR Read/Set
class CSRRS final : public Instruction {
public:
    CSRRS(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) override;

    static constexpr uint8_t getInstructionDescriptor() { return 0x2; }
};

// CSR Read/Clear
class CSRRC final : public Instruction {
public:
    CSRRC(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) override;

    static constexpr uint8_t getInstructionDescriptor() { return 0x3; }
};

// CSR Read/Write Immediate
class CSRRWI final : public Instruction {
public:
    CSRRWI(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) override;

    static constexpr uint8_t getInstructionDescriptor() { return 0x5; }
};

// CSR Read/Set Immediate
class CSRRSI final : public Instruction {
public:
    CSRRSI(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) override;

    static constexpr uint8_t getInstructionDescriptor() { return 0x6; }
};

// CSR Read/Clear Immediate
class CSRRCI final : public Instruction {
public:
    CSRRCI(uint32_t instruction) : Instruction(instruction) { decode(); }
    void execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) override;

    static constexpr uint8_t getInstructionDescriptor() { return 0x7; }
};

} // namespace Zicsr
