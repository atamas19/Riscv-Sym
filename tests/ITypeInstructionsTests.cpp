#include "RiscvCpuTest.h"

/////////////////////////
// I-Type Instructions //
/////////////////////////

// Arithmetic & Logical Immediate Instructions

TEST_F(RiscvCpuTest, AddiInstructionComputesCorrectly) {
    cpu->setRegister(1, 15);

    uint32_t encoded = AssemblyCompiler::compile("addi x2, x1, 25");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(2), 40);
}

TEST_F(RiscvCpuTest, AddiInstructionHandlesNegativeImmediate) {
    cpu->setRegister(1, 15);

    uint32_t encoded = AssemblyCompiler::compile("addi x2, x1, -5");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(2), 10);
}

TEST_F(RiscvCpuTest, SltiInstructionSignedComparison) {
    cpu->setRegister(1, static_cast<uint32_t>(-10));

    uint32_t encoded = AssemblyCompiler::compile("slti x2, x1, -5");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(2), 1);
}

TEST_F(RiscvCpuTest, SltiuInstructionUnsignedComparison) {
    cpu->setRegister(1, static_cast<uint32_t>(-10)); // 0xFFFFFFF6

    uint32_t encoded = AssemblyCompiler::compile("sltiu x2, x1, 15");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(2), 0);
}

TEST_F(RiscvCpuTest, XoriInstruction) {
    cpu->setRegister(1, 0b1010);

    uint32_t encoded = AssemblyCompiler::compile("xori x2, x1, 12");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(2), 0b0110);
}

TEST_F(RiscvCpuTest, OriInstruction) {
    cpu->setRegister(1, 0b1010);

    uint32_t encoded = AssemblyCompiler::compile("ori x2, x1, 12");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(2), 0b1110);
}

TEST_F(RiscvCpuTest, AndiInstruction) {
    cpu->setRegister(1, 0b1010);

    uint32_t encoded = AssemblyCompiler::compile("andi x2, x1, 12");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(2), 0b1000);
}

// Shift Immediate Instructions

TEST_F(RiscvCpuTest, SlliInstruction) {
    cpu->setRegister(1, 0b0011); // 3

    uint32_t encoded = AssemblyCompiler::compile("slli x2, x1, 4");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(2), 48);
}

TEST_F(RiscvCpuTest, SrliInstruction) {
    cpu->setRegister(1, 0xFFFFFFFF);

    uint32_t encoded = AssemblyCompiler::compile("srli x2, x1, 4");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(2), 0x0FFFFFFF);
}

TEST_F(RiscvCpuTest, SraiInstruction) {
    cpu->setRegister(1, 0xFFFFFFFF); // -1

    uint32_t encoded = AssemblyCompiler::compile("srai x2, x1, 4");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(2), 0xFFFFFFFF);
}

// Load Instructions

TEST_F(RiscvCpuTest, LwInstruction) {
    uint32_t address = 0x2000;
    Memory::getInstance().write32(address, 0x8FAABBCC);
    cpu->setRegister(1, address - 4);

    uint32_t encoded = AssemblyCompiler::compile("lw x2, 4(x1)");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(2), 0x8FAABBCC);
}

TEST_F(RiscvCpuTest, LhInstructionSignExtends) {
    uint32_t address = 0x2000;
    Memory::getInstance().write32(address, 0x8FAABBCC);
    cpu->setRegister(1, address);

    uint32_t encoded = AssemblyCompiler::compile("lh x2, 0(x1)");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(2), 0xFFFFBBCC);
}

TEST_F(RiscvCpuTest, LhuInstructionZeroExtends) {
    uint32_t address = 0x2000;
    Memory::getInstance().write32(address, 0x8FAABBCC);
    cpu->setRegister(1, address);

    uint32_t encoded = AssemblyCompiler::compile("lhu x2, 0(x1)");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(2), 0x0000BBCC);
}

TEST_F(RiscvCpuTest, LbInstructionSignExtends) {
    uint32_t address = 0x2000;
    Memory::getInstance().write32(address, 0x8FAABBCC);
    cpu->setRegister(1, address);

    uint32_t encoded = AssemblyCompiler::compile("lb x2, 0(x1)");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(2), 0xFFFFFFCC);
}

TEST_F(RiscvCpuTest, LbuInstructionZeroExtends) {
    uint32_t address = 0x2000;
    Memory::getInstance().write32(address, 0x8FAABBCC);
    cpu->setRegister(1, address);

    uint32_t encoded = AssemblyCompiler::compile("lbu x2, 0(x1)");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(2), 0x000000CC);
}

// Jump and Link Register (JALR)

TEST_F(RiscvCpuTest, JalrInstructionJumpsAndLinks) {
    uint32_t targetAddress = 0x4000;
    uint32_t currentPc = cpu->getPc();

    cpu->setRegister(1, targetAddress - 8);

    uint32_t encoded = AssemblyCompiler::compile("jalr x2, 8(x1)");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getPc(), targetAddress);
    EXPECT_EQ(cpu->getRegister(2), currentPc + 4);
}

// Fence Instructions (Executed as No-Ops)

TEST_F(RiscvCpuTest, FenceInstructionExecutesAsNoOp) {
    uint32_t currentPc = cpu->getPc();
    uint32_t encoded = 0x0000000F;

    auto inst = InstructionFactory::create(encoded);
    ASSERT_NE(inst, nullptr);
    inst->execute(*cpu);

    for (int i = 1; i < 32; i++) {
        EXPECT_EQ(cpu->getRegister(i), 0);
    }

    EXPECT_EQ(cpu->getPc(), currentPc + 8);
}

TEST_F(RiscvCpuTest, FenceIInstructionExecutesAsNoOp) {
    uint32_t currentPc = cpu->getPc();
    uint32_t encoded = 0x0000100F;

    auto inst = InstructionFactory::create(encoded);
    ASSERT_NE(inst, nullptr);
    inst->execute(*cpu);

    for (int i = 1; i < 32; i++) {
        EXPECT_EQ(cpu->getRegister(i), 0);
    }

    EXPECT_EQ(cpu->getPc(), currentPc + 4);
}
