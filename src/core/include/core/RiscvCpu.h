#pragma once

#include <memory>
#include <cstdint>
#include <array>

#include <core/instruction/Instruction.h>

#define DEBUG 0

class RiscvCpu {
public:
    static RiscvCpu& getInstance();

    int executeAsmCommand(const std::string& command, InstructionOutput& instructionOutput);

    void reset();

    bool executeFromBinFile(const std::string& filePath, uint32_t startAddr);

    // Getters
    const uint32_t getPc() const { return _pc; }
    const int32_t getRegister(uint8_t registerIndex) const;

    // Setters
    void setPc(uint32_t pcValue) { _pc = pcValue; }
    void setRegister(uint8_t registerIndex, int32_t registerValue);

private:
    RiscvCpu(): _mem(Memory::getInstance()) { reset(); };
    ~RiscvCpu() = default;
    RiscvCpu(const RiscvCpu&) = delete;
    RiscvCpu& operator=(const RiscvCpu&) = delete;
    RiscvCpu(RiscvCpu&&) = delete;
    RiscvCpu& operator=(RiscvCpu&&) = delete;

private:
    std::unique_ptr<Instruction> getInstructionFromAsmCommand(const std::string& asmCommand, InstructionOutput& instructionOutput);

    bool loadBinFileToMemory(const std::string& filename, uint32_t startAddr);

private:
    std::array<int32_t, 32> _regs;
    uint32_t _pc;
    Memory& _mem;
};
