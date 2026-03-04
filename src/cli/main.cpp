#include <core/RiscvCpu.h>

int main() {
    RiscvCpu& cpu = RiscvCpu::getInstance();

    Memory::getInstance().loadDiskImage("fs.img");

    cpu.executeFromBinFile("/home/atamas19/projects/dependencies/xv6-riscv-32bit/kernel/kernel.bin");

    return 0;
}
