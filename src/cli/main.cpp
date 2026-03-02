#include <core/RiscvCpu.h>

// constexpr const char* path = "/home/atamas19/projects/dependencies/xv6-riscv-32bit/kernel/kernel";

int main() {
    RiscvCpu& cpu = RiscvCpu::getInstance();

    // constexpr const char* path = "/home/atamas19/projects/randomStuff/elfTest/hello.bin";
    constexpr const char* path = "/home/atamas19/projects/dependencies/xv6-riscv-32bit/kernel/kernel.bin";

    Memory::getInstance().loadDiskImage("fs.img");

    cpu.executeFromBinFile(path);

    return 0;
}
