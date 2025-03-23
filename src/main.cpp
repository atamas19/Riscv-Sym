#include <iostream>

#include "RiscvCpu.h"

#include "instruction/JTypeInstruction.h"

int main()
{
    RiscvCpu& riscv = RiscvCpu::getInstance();

    riscv.run();
    
    return 0;
}