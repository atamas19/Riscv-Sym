#include "RiscvCpu.h"

RiscvCpu& RiscvCpu::getInstance()
{
    static RiscvCpu instance;

    return instance;
}

void RiscvCpu::run()
{
    while(true) // something
    {
        // fetch
        uint32_t instruction{ram.read32(pc)};
        // decode
        // execute
    }
}
