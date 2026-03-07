#include <core/RiscvCpu.h>

#ifndef _WIN32
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>

struct termios orig_termios;
int orig_stdin_flags;

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
    fcntl(STDIN_FILENO, F_SETFL, orig_stdin_flags);
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);

    orig_stdin_flags = fcntl(STDIN_FILENO, F_GETFL, 0);

    atexit(disableRawMode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);

    fcntl(STDIN_FILENO, F_SETFL, orig_stdin_flags | O_NONBLOCK);
}
#else
void enableRawMode() {
}
#endif

int main() {
    enableRawMode();

    RiscvCpu& cpu = RiscvCpu::getInstance();

    Memory::getInstance().loadDiskImage("fs.img");

    cpu.executeFromBinFile("/home/atamas19/projects/dependencies/xv6-riscv-32bit/kernel/kernel.bin");

    return 0;
}
