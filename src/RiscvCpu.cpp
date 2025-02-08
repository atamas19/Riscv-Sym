#include "RiscvCpu.h"

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
#ifdef DEBUG // this whole run function is just for checking implemented instructions for now
    uint32_t instruction{0b00000000001101101000011000000011};
    // 00000000101101100000011010110011
    // 00000000000101101000011000000011

    regs[13] = 0x00002650;

    std::cout << "Instruction in binary: " << std::bitset<32>(instruction) << std::endl;

    auto test = InstructionFactory::create(instruction);

    if (test) [[likely]]
        test->execute(*this);
    else [[unlikely]]
        std::cout << "Failed to create instruction" << std::endl;
#endif
}
