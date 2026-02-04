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

const int32_t RiscvCpu::getRegister(uint8_t registerIndex) const
{
    if (registerIndex >= 32)
        throw std::out_of_range("Register index out of range: x" + std::to_string(registerIndex));

    return regs[registerIndex];
}

void RiscvCpu::setRegister(uint8_t registerIndex, int32_t registerValue)
{
    if (registerIndex == 0) {
        return ;
    }
    if (registerIndex >= 32) {
        throw std::out_of_range("Register index out of range: x" + std::to_string(registerIndex));
    }

    regs[registerIndex] = registerValue;
}

int RiscvCpu::executeAsmCommand(const std::string& command, InstructionOutput& instructionOutput)
{
    auto instruction = getInstructionFromAsmCommand(command, instructionOutput);

    if (instruction == nullptr)
    {
        instructionOutput.exitCode = -1;
        return 1;
    }

    try
    {
        instruction->execute(*this, instructionOutput);
        instructionOutput.exitCode = 0;
    }
    catch(const std::exception& e)
    {
        instructionOutput.consoleLog = "Error executing `" + command + "`: " + std::string{e.what()};
        instructionOutput.exitCode = -1;
    }

    instruction.reset();

    return 0;
}

std::unique_ptr<Instruction> RiscvCpu::getInstructionFromAsmCommand(const std::string& asmCommand, InstructionOutput& instructionOutput)
{
    uint32_t binaryInstruction = AssemblyCompiler::compile(asmCommand, instructionOutput);

    std::cout << "Binary Instruction: " << std::bitset<32>(binaryInstruction) << "\n";

    if (binaryInstruction == 0)
    {
        instructionOutput.consoleLog = "Error converting `" + asmCommand + "` to binary: " + instructionOutput.consoleLog;
        return nullptr;
    }

    auto instruction = InstructionFactory::create(binaryInstruction);

    if (instruction == nullptr)
        instructionOutput.consoleLog = asmCommand + " doesn't exist";
        // Should never come into this if statement

    return instruction;
}

bool RiscvCpu::executeFromBinFile(const std::string& filePath) {
    if (!loadBinFileToMemory(filePath)) {
        std::cout << "Couldn't load bin file to memory!\n";
        return false;
    }

    std::cout << "Instructions have been loaded. Starting execution...\n";

    this->pc = 0x00000000;
    
    auto& mem = Memory::getInstance();
    InstructionOutput test;

    for (int i{0}; true; ++i) {
        #if DEBUG
        std::cout << "PC: " << std::to_string(this->pc) << " executed for " << i << "\n";
        #endif
        int32_t binaryInstruction = mem.read32(this->pc);

        if (binaryInstruction == 0) {
            std::cout << "End of code reached (0x0 instruction).\n";
            break;
        }

        auto instruction = InstructionFactory::create(binaryInstruction);

        if (!instruction) {
             std::cout << "Unknown instruction at PC: " << std::hex << pc << "\n";
             break;
        }
        
        instruction->execute(*this, test);

        #if DEBUG
        std::cout << "\nConsole log: " << test.consoleLog << "\n";
        #endif
    }

    return true;
}
bool RiscvCpu::loadBinFileToMemory(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) return false;

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        return false;
    }

    auto& mem = Memory::getInstance();
    uint32_t start_addr = 0x00000000;

    for(size_t i = 0; i < buffer.size(); i++) {
        mem.write8(start_addr + i, buffer[i]);
    }

    this->pc = start_addr; 
    
    return true;
}

void RiscvCpu::reset()
{
    pc = 0;
    std::fill(std::begin(regs), std::end(regs), 0);

    Memory::getInstance().reset();
}
