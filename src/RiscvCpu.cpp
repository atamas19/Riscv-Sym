#include "RiscvCpu.h"
#include "AssemblyCompiler.h"

#include <iostream>
#include <bitset>

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
    if (registerIndex == 0 && registerValue != 0) {
        throw std::invalid_argument("Cannot modify x0 (zero register).");
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

void RiscvCpu::reset()
{
    pc = 0;
    std::fill(std::begin(regs), std::end(regs), 0);

    Memory::getInstance().reset();
}
