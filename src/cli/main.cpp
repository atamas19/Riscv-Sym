#include <core/RiscvCpu.h>

#include <CLI/CLI.hpp>
#include <spdlog/spdlog.h>

#include <map>

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

int main(int argc, char** argv) {
    CLI::App app{"RISC-V 32-bit Emulator capable of booting xv6"};

    const std::map<std::string, spdlog::level::level_enum> log_level_map{
        {"trace", spdlog::level::trace},
        {"debug", spdlog::level::debug},
        {"info", spdlog::level::info},
        {"warn", spdlog::level::warn},
        {"error", spdlog::level::err},
        {"critical", spdlog::level::critical},
        {"off", spdlog::level::off}
    };

    std::string kernel_path;
    std::string disk_path;
    spdlog::level::level_enum log_level = spdlog::level::info;

    app.add_option("-k,--kernel", kernel_path, "Path to the kernel binary")->required();
    app.add_option("-d,--disk", disk_path, "Path to the filesystem disk image")->required();
    app.add_option("-l,--log-level", log_level, "Set the logging level")
       ->transform(CLI::CheckedTransformer(log_level_map, CLI::ignore_case));

    CLI11_PARSE(app, argc, argv);

    spdlog::set_level(log_level);
    spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");

    spdlog::info("Starting RISC-V Emulator...");
    spdlog::info("Kernel: {}", kernel_path);
    spdlog::info("Disk: {}", disk_path);

    enableRawMode();
    RiscvCpu& cpu = RiscvCpu::getInstance();

    try {
        if (Memory::getInstance().loadDiskImage(disk_path)) {
            spdlog::info("Booting...");
            if (!cpu.executeFromBinFile(kernel_path)) {
                spdlog::error("Failed to execute kernel binary: {}", kernel_path);
                return 1;
            }
        }
    } catch (const std::exception& e) {
        spdlog::error("Emulator crashed: {}", e.what());
        return 1;
    }

    return 0;
}
