#include "RiscvCpuTest.h"

/////////////////////////
// R-Type Instructions //
/////////////////////////

// RV32I Instructions

TEST_F(RiscvCpuTest, AddInstructionComputesCorrectly) {
    cpu->setRegister(1, 15);
    cpu->setRegister(2, 25);

    uint32_t encoded = AssemblyCompiler::compile("add x3, x1, x2");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 40);
    EXPECT_EQ(cpu->getPc(), 0x1004);
}

TEST_F(RiscvCpuTest, SubInstructionComputesCorrectly) {
    cpu->setRegister(1, 25);
    cpu->setRegister(2, 10);

    uint32_t encoded = AssemblyCompiler::compile("sub x3, x1, x2");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 15);
    EXPECT_EQ(cpu->getPc(), 0x1004);
}

TEST_F(RiscvCpuTest, SllInstructionShiftsLeft) {
    cpu->setRegister(1, 3);
    cpu->setRegister(2, 4);

    uint32_t encoded = AssemblyCompiler::compile("sll x3, x1, x2");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 48);
}

TEST_F(RiscvCpuTest, SltInstructionSignedComparison) {
    cpu->setRegister(1, static_cast<uint32_t>(-5));
    cpu->setRegister(2, 10);

    uint32_t encoded = AssemblyCompiler::compile("slt x3, x1, x2");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 1) << "-5 is less than 10";
}

TEST_F(RiscvCpuTest, SltuInstructionUnsignedComparison) {
    cpu->setRegister(1, static_cast<uint32_t>(-5));
    cpu->setRegister(2, 10);

    uint32_t encoded = AssemblyCompiler::compile("sltu x3, x1, x2");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 0) << "0xFFFFFFFB is NOT less than 10";
}

TEST_F(RiscvCpuTest, XorInstruction) {
    cpu->setRegister(1, 0b1010);
    cpu->setRegister(2, 0b1100);

    uint32_t encoded = AssemblyCompiler::compile("xor x3, x1, x2");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 0b0110);
}

TEST_F(RiscvCpuTest, SrlInstructionLogicalShiftRight) {
    cpu->setRegister(1, 0xFFFFFFFF);
    cpu->setRegister(2, 4);

    uint32_t encoded = AssemblyCompiler::compile("srl x3, x1, x2");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 0x0FFFFFFF);
}

TEST_F(RiscvCpuTest, SraInstructionArithmeticShiftRight) {
    cpu->setRegister(1, 0xFFFFFFFF);
    cpu->setRegister(2, 4);

    uint32_t encoded = AssemblyCompiler::compile("sra x3, x1, x2");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 0xFFFFFFFF);
}

TEST_F(RiscvCpuTest, OrInstruction) {
    cpu->setRegister(1, 0b1010);
    cpu->setRegister(2, 0b1100);

    uint32_t encoded = AssemblyCompiler::compile("or x3, x1, x2");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 0b1110);
}

TEST_F(RiscvCpuTest, AndInstruction) {
    cpu->setRegister(1, 0b1010);
    cpu->setRegister(2, 0b1100);

    uint32_t encoded = AssemblyCompiler::compile("and x3, x1, x2");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 0b1000);
}

// RV32M Instructions

TEST_F(RiscvCpuTest, MulInstruction_Standard) {
    cpu->setRegister(1, 5);
    cpu->setRegister(2, static_cast<uint32_t>(-4));

    uint32_t encoded = AssemblyCompiler::compile("mul x3, x1, x2");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), static_cast<uint32_t>(-20));
}

TEST_F(RiscvCpuTest, MulhInstruction_SignedXSigned) {
    cpu->setRegister(1, 0x7FFFFFFF);
    cpu->setRegister(2, 0x7FFFFFFF);

    uint32_t encoded = AssemblyCompiler::compile("mulh x3, x1, x2");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 0x3FFFFFFF) << "MULH (Signed * Signed) failed";
}

TEST_F(RiscvCpuTest, MulhsuInstruction_SignedXUnsigned) {
    cpu->setRegister(1, static_cast<uint32_t>(-2));
    cpu->setRegister(2, 0x80000000);

    uint32_t encoded = AssemblyCompiler::compile("mulhsu x3, x1, x2");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 0xFFFFFFFF) << "MULHSU (Signed * Unsigned) failed";
}

TEST_F(RiscvCpuTest, MulhuInstruction_UnsignedXUnsigned) {
    cpu->setRegister(1, 0xFFFFFFFF);
    cpu->setRegister(2, 0xFFFFFFFF);

    uint32_t encoded = AssemblyCompiler::compile("mulhu x3, x1, x2");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 0xFFFFFFFE) << "MULHU (Unsigned * Unsigned) failed";
}

TEST_F(RiscvCpuTest, DivInstruction_SignedNormal) {
    cpu->setRegister(1, static_cast<uint32_t>(-20));
    cpu->setRegister(2, 3);

    uint32_t encoded = AssemblyCompiler::compile("div x3, x1, x2");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), static_cast<uint32_t>(-6));
}

TEST_F(RiscvCpuTest, DivInstruction_DivideByZero) {
    cpu->setRegister(1, 100);
    cpu->setRegister(2, 0);

    uint32_t encoded = AssemblyCompiler::compile("div x3, x1, x2");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 0xFFFFFFFF) << "DIV by zero should return -1";
}

TEST_F(RiscvCpuTest, DivInstruction_Overflow) {
    cpu->setRegister(1, 0x80000000); // -2147483648
    cpu->setRegister(2, 0xFFFFFFFF); // -1

    uint32_t encoded = AssemblyCompiler::compile("div x3, x1, x2");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 0x80000000) << "DIV overflow should return the dividend";
}

TEST_F(RiscvCpuTest, DivuInstruction_Unsigned) {
    cpu->setRegister(1, 20);
    cpu->setRegister(2, 3);

    uint32_t encoded = AssemblyCompiler::compile("divu x3, x1, x2");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 6);
}

TEST_F(RiscvCpuTest, DivuInstruction_DivideByZero) {
    cpu->setRegister(1, 100);
    cpu->setRegister(2, 0);

    uint32_t encoded = AssemblyCompiler::compile("divu x3, x1, x2");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 0xFFFFFFFF) << "DIVU by zero should return 2^32 - 1";
}

TEST_F(RiscvCpuTest, RemInstruction_Signed) {
    cpu->setRegister(1, static_cast<uint32_t>(-20));
    cpu->setRegister(2, 3);

    uint32_t encoded = AssemblyCompiler::compile("rem x3, x1, x2");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), static_cast<uint32_t>(-2));
}

TEST_F(RiscvCpuTest, RemInstruction_DivideByZero) {
    cpu->setRegister(1, 100);
    cpu->setRegister(2, 0);

    uint32_t encoded = AssemblyCompiler::compile("rem x3, x1, x2");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 100) << "REM by zero should return the dividend";
}

TEST_F(RiscvCpuTest, RemInstruction_Overflow) {
    cpu->setRegister(1, 0x80000000); // -2147483648
    cpu->setRegister(2, 0xFFFFFFFF); // -1

    uint32_t encoded = AssemblyCompiler::compile("rem x3, x1, x2");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 0) << "REM overflow should return 0";
}

TEST_F(RiscvCpuTest, RemuInstruction_Unsigned) {
    cpu->setRegister(1, 20);
    cpu->setRegister(2, 3);

    uint32_t encoded = AssemblyCompiler::compile("remu x3, x1, x2");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 2);
}

TEST_F(RiscvCpuTest, RemuInstruction_DivideByZero) {
    cpu->setRegister(1, 100);
    cpu->setRegister(2, 0);

    uint32_t encoded = AssemblyCompiler::compile("remu x3, x1, x2");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 100) << "REMU by zero should return the dividend";
}

// RV32A Instructions

TEST_F(RiscvCpuTest, LrSc_SuccessFlow) {
    uint32_t targetAddress = 0x2000;
    Memory& mem = Memory::getInstance();
    mem.write32(targetAddress, 42);
    cpu->setRegister(1, targetAddress);

    uint32_t encodedLr = AssemblyCompiler::compile("lr.w x3, (x1)");
    InstructionFactory::create(encodedLr)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 42);

    cpu->setRegister(2, 100);
    uint32_t encodedSc = AssemblyCompiler::compile("sc.w x4, x2, (x1)");
    InstructionFactory::create(encodedSc)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(4), 0);
    EXPECT_EQ(mem.read32(targetAddress), 100);
}

TEST_F(RiscvCpuTest, LrSc_FailsOnInterveningStore) {
    uint32_t targetAddress = 0x3000;
    Memory& mem = Memory::getInstance();
    mem.write32(targetAddress, 10);

    cpu->setRegister(1, targetAddress);

    uint32_t encodedLr = AssemblyCompiler::compile("lr.w x3, (x1)");
    InstructionFactory::create(encodedLr)->execute(*cpu);

    cpu->setRegister(5, targetAddress);
    cpu->setRegister(6, 99);
    uint32_t encodedSw = AssemblyCompiler::compile("sw x6, 0(x5)");
    InstructionFactory::create(encodedSw)->execute(*cpu);

    cpu->setRegister(2, 100);
    uint32_t encodedSc = AssemblyCompiler::compile("sc.w x4, x2, (x1)");
    InstructionFactory::create(encodedSc)->execute(*cpu);

    EXPECT_NE(cpu->getRegister(4), 0);
    EXPECT_EQ(mem.read32(targetAddress), 99);
}

TEST_F(RiscvCpuTest, AmoSwapInstruction) {
    uint32_t targetAddress = 0x2000;
    Memory& mem = Memory::getInstance();
    mem.write32(targetAddress, 99);
    cpu->setRegister(1, targetAddress);
    cpu->setRegister(2, 77);

    uint32_t encoded = AssemblyCompiler::compile("amoswap.w x3, x2, (x1)");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 99);
    EXPECT_EQ(mem.read32(targetAddress), 77);
}

TEST_F(RiscvCpuTest, AmoAddInstruction) {
    uint32_t targetAddress = 0x2000;
    Memory& mem = Memory::getInstance();
    mem.write32(targetAddress, 50);
    cpu->setRegister(1, targetAddress);
    cpu->setRegister(2, 15);

    uint32_t encoded = AssemblyCompiler::compile("amoadd.w x3, x2, (x1)");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 50);
    EXPECT_EQ(mem.read32(targetAddress), 65);
}

TEST_F(RiscvCpuTest, AmoXorInstruction) {
    uint32_t targetAddress = 0x2000;
    Memory& mem = Memory::getInstance();
    mem.write32(targetAddress, 0xFFFFFFFF);
    cpu->setRegister(1, targetAddress);
    cpu->setRegister(2, 0x0000FFFF);

    uint32_t encoded = AssemblyCompiler::compile("amoxor.w x3, x2, (x1)");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 0xFFFFFFFF);
    EXPECT_EQ(mem.read32(targetAddress), 0xFFFF0000);
}

TEST_F(RiscvCpuTest, AmoAndInstruction) {
    uint32_t targetAddress = 0x2000;
    Memory& mem = Memory::getInstance();
    mem.write32(targetAddress, 0b11110000);
    cpu->setRegister(1, targetAddress);
    cpu->setRegister(2, 0b10101010);

    uint32_t encoded = AssemblyCompiler::compile("amoand.w x3, x2, (x1)");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 0b11110000);
    EXPECT_EQ(mem.read32(targetAddress), 0b10100000);
}

TEST_F(RiscvCpuTest, AmoOrInstruction) {
    uint32_t targetAddress = 0x2000;
    Memory& mem = Memory::getInstance();
    mem.write32(targetAddress, 0b11110000);
    cpu->setRegister(1, targetAddress);
    cpu->setRegister(2, 0b00001111);

    uint32_t encoded = AssemblyCompiler::compile("amoor.w x3, x2, (x1)");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 0b11110000);
    EXPECT_EQ(mem.read32(targetAddress), 0b11111111);
}

TEST_F(RiscvCpuTest, AmoMinInstruction) {
    uint32_t targetAddress = 0x2000;
    Memory& mem = Memory::getInstance();
    mem.write32(targetAddress, 10);
    cpu->setRegister(1, targetAddress);
    cpu->setRegister(2, static_cast<uint32_t>(-50));

    uint32_t encoded = AssemblyCompiler::compile("amomin.w x3, x2, (x1)");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 10);
    EXPECT_EQ(mem.read32(targetAddress), static_cast<uint32_t>(-50));
}

TEST_F(RiscvCpuTest, AmoMaxInstruction) {
    uint32_t targetAddress = 0x2000;
    Memory& mem = Memory::getInstance();
    mem.write32(targetAddress, 10);
    cpu->setRegister(1, targetAddress);
    cpu->setRegister(2, static_cast<uint32_t>(-50));

    uint32_t encoded = AssemblyCompiler::compile("amomax.w x3, x2, (x1)");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 10);
    EXPECT_EQ(mem.read32(targetAddress), 10);
}

TEST_F(RiscvCpuTest, AmoMinuInstruction) {
    uint32_t targetAddress = 0x2000;
    Memory& mem = Memory::getInstance();
    mem.write32(targetAddress, 10);
    cpu->setRegister(1, targetAddress);
    cpu->setRegister(2, static_cast<uint32_t>(-50));

    uint32_t encoded = AssemblyCompiler::compile("amominu.w x3, x2, (x1)");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 10);
    EXPECT_EQ(mem.read32(targetAddress), 10);
}

TEST_F(RiscvCpuTest, AmoMaxuInstruction) {
    uint32_t targetAddress = 0x2000;
    Memory& mem = Memory::getInstance();
    mem.write32(targetAddress, 10);
    cpu->setRegister(1, targetAddress);
    cpu->setRegister(2, static_cast<uint32_t>(-50));

    uint32_t encoded = AssemblyCompiler::compile("amomaxu.w x3, x2, (x1)");
    InstructionFactory::create(encoded)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 10);
    EXPECT_EQ(mem.read32(targetAddress), static_cast<uint32_t>(-50));
}
