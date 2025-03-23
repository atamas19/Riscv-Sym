#pragma once

#include <memory>
#include <cstdint>

#define DEBUG 1

class RiscvCpu
{
public:
    static RiscvCpu& getInstance();

    void run();

    // Getters
    const uint32_t getPc() const { return pc; }
    const int32_t getRegister(uint8_t registerIndex) const;

    // Setters
    void setPc(uint32_t pcValue) { pc = pcValue; }
    void setRegister(uint8_t registerIndex, int32_t registerValue);

private:
    RiscvCpu() = default;
    ~RiscvCpu() = default;
        // fetch - decode - execute
    RiscvCpu(const RiscvCpu&) = delete;
    RiscvCpu& operator=(const RiscvCpu&) = delete;
    RiscvCpu(RiscvCpu&&) = delete;
    RiscvCpu& operator=(RiscvCpu&&) = delete;
private:
    int32_t regs[32];
    uint32_t pc;
};
