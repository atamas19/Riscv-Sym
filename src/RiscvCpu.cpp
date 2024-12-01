#include "RiscvCpu.h"

#include <iostream>
#include <bitset>
#include <cassert>

RiscvCpu& RiscvCpu::getInstance()
{
    static RiscvCpu instance;

    return instance;
}

const uint32_t RiscvCpu::getRegister(uint8_t registerIndex) const
{
    assert(registerIndex < 32);

    return regs[registerIndex];
}

void RiscvCpu::setRegister(uint8_t registerIndex, uint32_t registerValue)
{
    assert(registerIndex < 32);
    assert(registerIndex > 0);

    regs[registerIndex] = registerValue;
}

void RiscvCpu::run()
{
#ifdef DEBUG // this whole run function is just for checking implemented instructions for now
    uint32_t instruction{0b00000000001000011001001000110011};

    regs[3] = 0x12345678;
    regs[2] = 0x08;

    std::cout << "Instruction in binary: " << std::bitset<32>(instruction) << std::endl;

    auto test = InstructionFactory::create(instruction);

    if (test) [[likely]]
        test->execute(*this);
    else [[unlikely]]
        std::cout << "Failed to create instruction" << std::endl;
#endif
}
