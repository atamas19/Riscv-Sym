#include "RiscvCpu.h"

std::unique_ptr<RiscvCpu> RiscvCpu::instance = nullptr;

RiscvCpu& RiscvCpu::getInstance()
{
    if (instance == nullptr)
        instance.reset(new RiscvCpu);
    
    return *instance;
}