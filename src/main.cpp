#include <iostream>

#include "RiscvCpu.h"

int main()
{
    RiscvCpu& riscv = RiscvCpu::getInstance();

    riscv.run();
    
    return 0;
}