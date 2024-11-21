#pragma once

#include <memory>
#include <cstdint>

#include "Registers.h"
#include "Ram.h"
#include "instruction/Instruction.h"

class RiscvCpu
{
public:
    static RiscvCpu& getInstance();

    void run();

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
    Registers registers;
    uint32_t pc;
};
