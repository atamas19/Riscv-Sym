#include "RiscvCpu.h"
#include "instruction/RTypeInstruction.h"

#include <iostream>
#include <bitset>

RiscvCpu& RiscvCpu::getInstance()
{
    static RiscvCpu instance;

    return instance;
}

void RiscvCpu::run()
{
    // while(true) // something
    // {
        // fetch
    uint32_t instruction{0b00000000001000011000001000110011};

    // Print the instruction in binary
    std::cout << "Instruction in binary: " << std::bitset<32>(instruction) << std::endl;

    // Create an instruction using the factory method
    auto test = InstructionFactory::create(instruction);

    if (test)
        test->execute(RiscvCpu::getInstance());
    else
        std::cout << "Failed to create instruction" << std::endl;

        // test->execute(*this);
    // }
}
