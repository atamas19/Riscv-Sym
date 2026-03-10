#include <core/RiscvCpu.h>

#ifndef _WIN32
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>

struct termios orig_termios;
int orig_stdin_flags;

static bool raw_mode_enabled = false;

void disableRawMode() {
    if (!raw_mode_enabled) {
        return;
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
    fcntl(STDIN_FILENO, F_SETFL, orig_stdin_flags);
    raw_mode_enabled = false;
}

void enableRawMode() {
    if (raw_mode_enabled) {
        return;
    }
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
        return;
    }
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    if (flags == -1) {
        return;
    }
    orig_stdin_flags = flags;
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == -1) {
        return;
    }
    if (fcntl(STDIN_FILENO, F_SETFL, orig_stdin_flags | O_NONBLOCK) == -1) {
        // Attempt to restore original terminal attributes on failure.
        tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
        return;
    }
    raw_mode_enabled = true;
    atexit(disableRawMode);
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
