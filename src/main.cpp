#include <iostream>

#include "RiscvCpu.h"

int main()
{
    RiscvCpu& riscv = RiscvCpu::getInstance();

    riscv.run();

    uint32_t a = 1;

    std::cout << "\nTEST: " << ((a << 4) | 0xF);
    
    return 0;
}