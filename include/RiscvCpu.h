#pragma once

#include <memory>
#include <cstdint>

#include "Registers.h"
#include "Ram.h"

class RiscvCpu
{
public:
    static RiscvCpu& getInstance();

    ~RiscvCpu() = default;
private:
    RiscvCpu() = default;

    RiscvCpu(const RiscvCpu&) = delete;
    RiscvCpu& operator=(const RiscvCpu&) = delete;
    RiscvCpu(RiscvCpu&&) = delete;
    RiscvCpu& operator=(RiscvCpu&&) = delete;
private:
    Ram ram{1024};
    Registers registers;
    uint32_t pc;

    static std::unique_ptr<RiscvCpu> instance;
};