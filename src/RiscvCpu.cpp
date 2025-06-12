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

void RiscvCpu::run()
{
#if DEBUG // this whole run function is just for checking implemented instructions for now
    uint32_t instruction{0b01010101011011111111001111101111};

    regs[5] = 0x1;
    regs[6] = 0x1;

    std::cout << "Instruction in binary: " << std::bitset<32>(instruction) << std::endl;

    auto test = InstructionFactory::create(instruction);

    if (test) [[likely]]
        // test->execute(*this);
        ;
    else [[unlikely]]
        std::cout << "Failed to create instruction" << std::endl;

    test.reset();
#endif
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
        instructionOutput.consoleLog = asmCommand + " doesn't exist"; // TODO: find better wording
        // Should never come into this if statement

    return instruction;
}

void RiscvCpu::reset()
{
    pc = 0;
    std::fill(std::begin(regs), std::end(regs), 0);

    Memory::getInstance().reset();
}
