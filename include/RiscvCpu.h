#pragma once

#include <memory>
#include <cstdint>

#include "Ram.h"
#include "instruction/Instruction.h"

#define DEBUG

class RiscvCpu
{
public:
    static RiscvCpu& getInstance();

    void run();

    // Getters
    Ram& getRam() { return ram; }
    const uint32_t getPc() const { return pc; }
    const uint32_t getRegister(uint8_t registerIndex) const;

    // Setters
    void setPc(uint32_t pcValue) { pc = pcValue; }
    void setRegister(uint8_t registerIndex, uint32_t registerValue);

private:
    RiscvCpu() = default;
    ~RiscvCpu() = default;
        // fetch - decode - execute
    RiscvCpu(const RiscvCpu&) = delete;
    RiscvCpu& operator=(const RiscvCpu&) = delete;
    RiscvCpu(RiscvCpu&&) = delete;
    RiscvCpu& operator=(RiscvCpu&&) = delete;
private:
    Ram ram{1024};
    uint32_t regs[32];
    uint32_t pc;
};
