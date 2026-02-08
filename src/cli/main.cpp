#include <core/RiscvCpu.h>

int main() {
    RiscvCpu& cpu = RiscvCpu::getInstance();

    constexpr const char* path = "/home/atamas19/projects/randomStuff/elfTest/hello.bin";

    cpu.executeFromBinFile(path);

    return 0;
}
