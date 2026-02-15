#include <core/RiscvCpu.h>
#include <core/AssemblyCompiler.h>

#include <iostream>
#include <bitset>
#include <fstream>

RiscvCpu& RiscvCpu::getInstance()
{
    static RiscvCpu instance;

    return instance;
}

uint32_t RiscvCpu::getPc() const {
    return _pc;
}

uint32_t RiscvCpu::getRegister(uint8_t registerIndex) const {
    return _regs.at(registerIndex);
}

CsrUnit& RiscvCpu::getCsr() {
    return _csrUnit;
}

PrivilegeMode RiscvCpu::getPrivilegeMode() const {
    return _priviledgeMode;
}

void RiscvCpu::setPc(uint32_t pcValue) {
    _pc = pcValue;
}

void RiscvCpu::setRegister(uint8_t registerIndex, uint32_t registerValue) {
    if (registerIndex == 0) {
        return ;
    }

    _regs.at(registerIndex) = registerValue;
}

void RiscvCpu::setPrivilegeMode(PrivilegeMode mode) {
    _priviledgeMode = mode;
}

int RiscvCpu::executeAsmCommand(const std::string& command, InstructionOutput& instructionOutput) {
    auto instruction = getInstructionFromAsmCommand(command, instructionOutput);

    if (instruction == nullptr) {
        instructionOutput.exitCode = -1;
        return 1;
    }

    try {
        instruction->execute(*this, instructionOutput);
        instructionOutput.exitCode = 0;
    } catch(const std::exception& e) {
        instructionOutput.consoleLog = "Error executing `" + command + "`: " + std::string{e.what()};
        instructionOutput.exitCode = -1;
    }

    instruction.reset();

    return 0;
}

std::unique_ptr<Instruction> RiscvCpu::getInstructionFromAsmCommand(const std::string& asmCommand, InstructionOutput& instructionOutput) {
    uint32_t binaryInstruction = AssemblyCompiler::compile(asmCommand, instructionOutput);

    std::cout << "Binary Instruction: " << std::bitset<32>(binaryInstruction) << "\n";

    if (binaryInstruction == 0) {
        instructionOutput.consoleLog = "Error converting `" + asmCommand + "` to binary: " + instructionOutput.consoleLog;
        return nullptr;
    }

    auto instruction = InstructionFactory::create(binaryInstruction);

    if (instruction == nullptr)
        instructionOutput.consoleLog = asmCommand + " doesn't exist";
        // Should never come into this if statement

    return instruction;
}

bool RiscvCpu::executeFromBinFile(const std::string& filePath, uint32_t startAddr) {
    if (!loadBinFileToMemory(filePath, startAddr)) {
        std::cout << "Couldn't load bin file to memory!\n";
        return false;
    }

    std::cout << "Instructions have been loaded. Starting execution...\n";

    this->_pc = startAddr;

    InstructionOutput test;

    for (int i{0}; true; ++i) {
        #if DEBUG
        std::cout << "PC: " << std::to_string(this->_pc) << " executed for " << i << "\n";
        #endif
        uint32_t binaryInstruction = _mem.read32(this->_pc);

        if (binaryInstruction == 0) {
            std::cout << "End of code reached (0x0 instruction).\n";
            break;
        }

        auto instruction = InstructionFactory::create(binaryInstruction);

        if (!instruction) {
             std::cout << "Unknown instruction at PC: " << std::hex << _pc << "\n";
             break;
        }
        
        instruction->execute(*this, test);

        #if DEBUG
        std::cout << "\nConsole log: " << test.consoleLog << "\n";
        #endif
    }

    return true;
}

bool RiscvCpu::loadBinFileToMemory(const std::string& filename, uint32_t startAddr) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) return false;

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        return false;
    }

    for(size_t i = 0; i < buffer.size(); i++) {
        // TODO: consider loading bigger blocks of memory in the future
        _mem.write8(startAddr + i, buffer[i]);
    }
    
    return true;
}

void RiscvCpu::reset() {
    constexpr uint32_t ram_base = 0x80000000;
    constexpr uint32_t stack_size = 1024 * 1024;

    _priviledgeMode = PrivilegeMode::Machine;

    _regs.fill(0);
    _pc = ram_base;
    _regs.at(2) = ram_base + stack_size;

    _mem.reset();
    _csrUnit.reset();
}

void RiscvCpu::resetGUI() {
    _regs.fill(0);
    _pc = 0;
    _mem.reset();
}
