#include "RiscvCpuTest.h"

/////////////////////////
// S-Type Instructions //
/////////////////////////

TEST_F(RiscvCpuTest, SwInstructionStoresWord) {
    uint32_t address = 0x2000;
    cpu->setRegister(1, address);
    cpu->setRegister(2, 0x11223344);

    uint32_t encoded = AssemblyCompiler::compile("sw x2, 0(x1)");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(Memory::getInstance().read32(address), 0x11223344);
}

TEST_F(RiscvCpuTest, SwInstructionWithPositiveOffset) {
    uint32_t address = 0x2000;
    cpu->setRegister(1, address);
    cpu->setRegister(2, 0x55667788);

    uint32_t encoded = AssemblyCompiler::compile("sw x2, 16(x1)");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(Memory::getInstance().read32(address + 16), 0x55667788);
}

TEST_F(RiscvCpuTest, SwInstructionWithNegativeOffset) {
    uint32_t address = 0x2000;
    cpu->setRegister(1, address);
    cpu->setRegister(2, 0x99AABBCC);

    uint32_t encoded = AssemblyCompiler::compile("sw x2, -4(x1)");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(Memory::getInstance().read32(address - 4), 0x99AABBCC);
}

TEST_F(RiscvCpuTest, ShInstructionStoresHalfword) {
    uint32_t address = 0x2000;
    Memory::getInstance().write32(address, 0xFFFFFFFF);

    cpu->setRegister(1, address);
    cpu->setRegister(2, 0x1234AABB);

    uint32_t encoded = AssemblyCompiler::compile("sh x2, 0(x1)");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(Memory::getInstance().read32(address), 0xFFFFAABB);
}

TEST_F(RiscvCpuTest, ShInstructionWithOffset) {
    uint32_t address = 0x2000;
    Memory::getInstance().write32(address + 2, 0xFFFFFFFF);

    cpu->setRegister(1, address);
    cpu->setRegister(2, 0x1234AABB);

    uint32_t encoded = AssemblyCompiler::compile("sh x2, 2(x1)");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(Memory::getInstance().read32(address + 2), 0xFFFFAABB);
}

TEST_F(RiscvCpuTest, SbInstructionStoresByte) {
    uint32_t address = 0x2000;
    Memory::getInstance().write32(address, 0xFFFFFFFF);

    cpu->setRegister(1, address);
    cpu->setRegister(2, 0x123456AA);

    uint32_t encoded = AssemblyCompiler::compile("sb x2, 0(x1)");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(Memory::getInstance().read32(address), 0xFFFFFFAA);
}

TEST_F(RiscvCpuTest, SbInstructionWithOffset) {
    uint32_t address = 0x2000;
    Memory::getInstance().write32(address + 1, 0xFFFFFFFF);

    cpu->setRegister(1, address);
    cpu->setRegister(2, 0x123456AA);

    uint32_t encoded = AssemblyCompiler::compile("sb x2, 1(x1)");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(Memory::getInstance().read32(address + 1), 0xFFFFFFAA);
}
