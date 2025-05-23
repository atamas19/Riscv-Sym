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
        test->execute(*this);
    else [[unlikely]]
        std::cout << "Failed to create instruction" << std::endl;

    test.reset();
#endif
}

// TODO: replace int with a custom output, with an exit code, an output message, maybe a list of the affected registers, pc, ram, or I will put an observer
// on the RiscvCpu class that will be notified when registers or pc will be modified
// a sepparate observer would be needed for the Memory class
int RiscvCpu::executeAsmCommand(const std::string& command)
{
    auto instruction = getInstructionFromAsmCommand(command);

    if (instruction == nullptr)
        return 1;

    instruction->execute(*this);

    instruction.reset();

    return 0;
}

std::unique_ptr<Instruction> RiscvCpu::getInstructionFromAsmCommand(const std::string& asmCommand)
{
    uint32_t binaryInstruction = AssemblyCompiler::compile(asmCommand);

    if (binaryInstruction == 0)
        // means stuff is not right
        return nullptr;

    auto instruction = InstructionFactory::create(binaryInstruction);

    return instruction;
}

