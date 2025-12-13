#pragma once

#include <memory>
#include <cstdint>

#include <core/instruction/Instruction.h>

#define DEBUG 1

class RiscvCpu
{
public:
    static RiscvCpu& getInstance();

    int executeAsmCommand(const std::string& command, InstructionOutput& instructionOutput);

    void reset();

    // Getters
    const uint32_t getPc() const { return pc; }
    const int32_t getRegister(uint8_t registerIndex) const;

    // Setters
    void setPc(uint32_t pcValue) { pc = pcValue; }
    void setRegister(uint8_t registerIndex, int32_t registerValue);

private:
    RiscvCpu() = default;
    ~RiscvCpu() = default;
    RiscvCpu(const RiscvCpu&) = delete;
    RiscvCpu& operator=(const RiscvCpu&) = delete;
    RiscvCpu(RiscvCpu&&) = delete;
    RiscvCpu& operator=(RiscvCpu&&) = delete;

private:
    std::unique_ptr<Instruction> getInstructionFromAsmCommand(const std::string& asmCommand, InstructionOutput& instructionOutput);

private:
    int32_t regs[32];
    uint32_t pc;
};
