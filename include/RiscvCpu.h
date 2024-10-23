#pragma once

#include "Registers.h"

#include <memory>

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
    Registers registers;
    int32_t pc;

    static std::unique_ptr<RiscvCpu> instance;
};