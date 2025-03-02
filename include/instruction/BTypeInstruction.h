#pragma once

#include "Instruction.h"

namespace BType
{

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


// class SB : public Instruction
// {
// public:
//     SB(uint32_t instruction) : Instruction(instruction) { decode(); }
//     void execute(RiscvCpu& cpu) override;

//     static const uint8_t getInstructionDescriptor() { return 0x0; }
// };

} // namespace BType