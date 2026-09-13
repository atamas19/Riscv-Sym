#include "RiscvCpuTest.h"

/////////////////////////
// B-Type Instructions //
/////////////////////////

TEST_F(RiscvCpuTest, BeqInstructionTakesBranchWhenEqual) {
    uint32_t currentPc = cpu->getPc();
    cpu->setRegister(1, 42);
    cpu->setRegister(2, 42);

    uint32_t encoded = AssemblyCompiler::compile("beq x1, x2, 16");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(cpu->getNextPc(), currentPc + 16);
}

TEST_F(RiscvCpuTest, BeqInstructionDoesNotBranchWhenNotEqual) {
    const uint32_t currentNextPc = cpu->getNextPc();
    cpu->setRegister(1, 42);
    cpu->setRegister(2, 99);

    uint32_t encoded = AssemblyCompiler::compile("beq x1, x2, 16");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(cpu->getNextPc(), currentNextPc);
}

TEST_F(RiscvCpuTest, BeqInstructionHandlesNegativeOffset) {
    uint32_t currentPc = cpu->getPc();
    cpu->setRegister(1, 42);
    cpu->setRegister(2, 42);

    uint32_t encoded = AssemblyCompiler::compile("beq x1, x2, -16");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(cpu->getNextPc(), currentPc - 16);
}

TEST_F(RiscvCpuTest, BneInstructionTakesBranchWhenNotEqual) {
    uint32_t currentPc = cpu->getPc();
    cpu->setRegister(1, 10);
    cpu->setRegister(2, 20);

    uint32_t encoded = AssemblyCompiler::compile("bne x1, x2, 32");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(cpu->getNextPc(), currentPc + 32);
}

TEST_F(RiscvCpuTest, BneInstructionDoesNotBranchWhenEqual) {
    const uint32_t currentNextPc = cpu->getNextPc();
    cpu->setRegister(1, 10);
    cpu->setRegister(2, 10);

    uint32_t encoded = AssemblyCompiler::compile("bne x1, x2, 32");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(cpu->getNextPc(), currentNextPc);
}

TEST_F(RiscvCpuTest, BltInstructionTakesBranchWhenSignedLessThan) {
    uint32_t currentPc = cpu->getPc();
    cpu->setRegister(1, static_cast<uint32_t>(-15));
    cpu->setRegister(2, 5);

    uint32_t encoded = AssemblyCompiler::compile("blt x1, x2, 16");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(cpu->getNextPc(), currentPc + 16);
}

TEST_F(RiscvCpuTest, BltInstructionDoesNotBranchWhenSignedGreaterOrEqual) {
    const uint32_t currentNextPc = cpu->getNextPc();
    cpu->setRegister(1, 5);
    cpu->setRegister(2, static_cast<uint32_t>(-15));

    uint32_t encoded = AssemblyCompiler::compile("blt x1, x2, 16");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(cpu->getNextPc(), currentNextPc);
}

TEST_F(RiscvCpuTest, BgeInstructionTakesBranchWhenSignedGreater) {
    uint32_t currentPc = cpu->getPc();
    cpu->setRegister(1, 5);
    cpu->setRegister(2, static_cast<uint32_t>(-15));

    uint32_t encoded = AssemblyCompiler::compile("bge x1, x2, 16");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(cpu->getNextPc(), currentPc + 16);
}

TEST_F(RiscvCpuTest, BgeInstructionTakesBranchWhenEqual) {
    uint32_t currentPc = cpu->getPc();
    cpu->setRegister(1, static_cast<uint32_t>(-10));
    cpu->setRegister(2, static_cast<uint32_t>(-10));

    uint32_t encoded = AssemblyCompiler::compile("bge x1, x2, 16");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(cpu->getNextPc(), currentPc + 16);
}

TEST_F(RiscvCpuTest, BgeInstructionDoesNotBranchWhenSignedLess) {
    const uint32_t currentNextPc = cpu->getNextPc();
    cpu->setRegister(1, static_cast<uint32_t>(-15));
    cpu->setRegister(2, 5);

    uint32_t encoded = AssemblyCompiler::compile("bge x1, x2, 16");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(cpu->getNextPc(), currentNextPc);
}

TEST_F(RiscvCpuTest, BltuInstructionTakesBranchWhenUnsignedLessThan) {
    uint32_t currentPc = cpu->getPc();
    cpu->setRegister(1, 5);
    cpu->setRegister(2, 10);

    uint32_t encoded = AssemblyCompiler::compile("bltu x1, x2, 16");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(cpu->getNextPc(), currentPc + 16);
}

TEST_F(RiscvCpuTest, BltuInstructionDoesNotBranchOnNegativeSignedValue) {
    const uint32_t currentNextPc = cpu->getNextPc();
    cpu->setRegister(1, static_cast<uint32_t>(-15));
    cpu->setRegister(2, 5);

    uint32_t encoded = AssemblyCompiler::compile("bltu x1, x2, 16");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(cpu->getNextPc(), currentNextPc);
}

TEST_F(RiscvCpuTest, BgeuInstructionTakesBranchOnNegativeSignedValue) {
    uint32_t currentPc = cpu->getPc();
    cpu->setRegister(1, static_cast<uint32_t>(-15));
    cpu->setRegister(2, 5);

    uint32_t encoded = AssemblyCompiler::compile("bgeu x1, x2, 16");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(cpu->getNextPc(), currentPc + 16);
}

TEST_F(RiscvCpuTest, BgeuInstructionDoesNotBranchWhenUnsignedLess) {
    const uint32_t currentNextPc = cpu->getNextPc();
    cpu->setRegister(1, 5);
    cpu->setRegister(2, 10);

    uint32_t encoded = AssemblyCompiler::compile("bgeu x1, x2, 16");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(cpu->getNextPc(), currentNextPc);
}

TEST_F(RiscvCpuTest, ExecuteBEQWithOffset) {
    uint32_t startPc = 0xC00AF6DA;
    cpu->setPc(startPc);
    cpu->setRegister(10, 0);

    uint32_t encoded = AssemblyCompiler::compile("beq x10, x0, 22");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(cpu->getNextPc(), startPc + 22);
}
