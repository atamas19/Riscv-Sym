#include "RiscvCpu.h"
#include "AssemblyCompiler.h"

#include <iostream>
#include <bitset>
#include <cassert>

RiscvCpu& RiscvCpu::getInstance()
{
    static RiscvCpu instance;

    return instance;
}

const int32_t RiscvCpu::getRegister(uint8_t registerIndex) const
{
    assert(registerIndex < 32);

    return regs[registerIndex];
}

void RiscvCpu::setRegister(uint8_t registerIndex, int32_t registerValue)
{
    assert(registerIndex < 32);
    assert(registerIndex > 0);

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

    instruction->execute(*this, instructionOutput);

    instructionOutput.exitCode = 0;

    instruction.reset();

    return 0;
}

std::unique_ptr<Instruction> RiscvCpu::getInstructionFromAsmCommand(const std::string& asmCommand, InstructionOutput& instructionOutput)
{
    uint32_t binaryInstruction = AssemblyCompiler::compile(asmCommand);

    if (binaryInstruction == 0)
    {
        instructionOutput.consoleLog = asmCommand + " couldn't be converted to binary";
        return nullptr;
    }

    auto instruction = InstructionFactory::create(binaryInstruction);

    if (instruction == nullptr)
        instructionOutput.consoleLog = asmCommand + " doesn't exist"; // TODO: find better wording

    return instruction;
}

