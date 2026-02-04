#pragma once

#include <memory>
#include <cstdint>

#include <core/instruction/Instruction.h>

#define DEBUG 0

class RiscvCpu
{
public:
    static RiscvCpu& getInstance();

    int executeAsmCommand(const std::string& command, InstructionOutput& instructionOutput);

    void reset();

    bool executeFromBinFile(const std::string& filePath);

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

    bool loadBinFileToMemory(const std::string& filename);

private:
    int32_t regs[32];
    uint32_t pc;
};
