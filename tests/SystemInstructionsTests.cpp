#include "RiscvCpuTest.h"

TEST_F(RiscvCpuTest, CsrRwInstructionWritesAndReads) {
    cpu->setRegister(1, 0x11223344);

    uint32_t encodedWrite = AssemblyCompiler::compile("csrrw x0, mscratch, x1");
    InstructionFactory::create(encodedWrite)->execute(*cpu);

    uint32_t encodedRead = AssemblyCompiler::compile("csrrw x2, mscratch, x0");
    InstructionFactory::create(encodedRead)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(2), 0x11223344);
}

TEST_F(RiscvCpuTest, CsrRsInstructionSetsBits) {
    cpu->setRegister(1, 0b1010);
    uint32_t encodedWrite = AssemblyCompiler::compile("csrrw x0, mscratch, x1");
    InstructionFactory::create(encodedWrite)->execute(*cpu);

    cpu->setRegister(2, 0b0110);
    uint32_t encodedSet = AssemblyCompiler::compile("csrrs x3, mscratch, x2");
    InstructionFactory::create(encodedSet)->execute(*cpu);

    uint32_t encodedRead = AssemblyCompiler::compile("csrrw x4, mscratch, x0");
    InstructionFactory::create(encodedRead)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 0b1010);
    EXPECT_EQ(cpu->getRegister(4), 0b1110);
}

TEST_F(RiscvCpuTest, CsrRcInstructionClearsBits) {
    cpu->setRegister(1, 0b1111);
    uint32_t encodedWrite = AssemblyCompiler::compile("csrrw x0, mscratch, x1");
    InstructionFactory::create(encodedWrite)->execute(*cpu);

    cpu->setRegister(2, 0b0101);
    uint32_t encodedClear = AssemblyCompiler::compile("csrrc x3, mscratch, x2");
    InstructionFactory::create(encodedClear)->execute(*cpu);

    uint32_t encodedRead = AssemblyCompiler::compile("csrrw x4, mscratch, x0");
    InstructionFactory::create(encodedRead)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(3), 0b1111);
    EXPECT_EQ(cpu->getRegister(4), 0b1010);
}

TEST_F(RiscvCpuTest, CsrRwiInstructionWritesImmediate) {
    uint32_t encodedWriteImm = AssemblyCompiler::compile("csrrwi x0, mscratch, 15");
    InstructionFactory::create(encodedWriteImm)->execute(*cpu);

    uint32_t encodedRead = AssemblyCompiler::compile("csrrw x2, mscratch, x0");
    InstructionFactory::create(encodedRead)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(2), 15);
}

TEST_F(RiscvCpuTest, CsrRsiInstructionSetsBitsImmediate) {
    cpu->setRegister(1, 0b1000);
    uint32_t encodedWrite = AssemblyCompiler::compile("csrrw x0, mscratch, x1");
    InstructionFactory::create(encodedWrite)->execute(*cpu);

    uint32_t encodedSetImm = AssemblyCompiler::compile("csrrsi x2, mscratch, 7");
    InstructionFactory::create(encodedSetImm)->execute(*cpu);

    uint32_t encodedRead = AssemblyCompiler::compile("csrrw x3, mscratch, x0");
    InstructionFactory::create(encodedRead)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(2), 0b1000);
    EXPECT_EQ(cpu->getRegister(3), 0b1111);
}

TEST_F(RiscvCpuTest, CsrRciInstructionClearsBitsImmediate) {
    cpu->setRegister(1, 0b1111);
    uint32_t encodedWrite = AssemblyCompiler::compile("csrrw x0, mscratch, x1");
    InstructionFactory::create(encodedWrite)->execute(*cpu);

    uint32_t encodedClearImm = AssemblyCompiler::compile("csrrci x2, mscratch, 3");
    InstructionFactory::create(encodedClearImm)->execute(*cpu);

    uint32_t encodedRead = AssemblyCompiler::compile("csrrw x3, mscratch, x0");
    InstructionFactory::create(encodedRead)->execute(*cpu);

    EXPECT_EQ(cpu->getRegister(2), 0b1111);
    EXPECT_EQ(cpu->getRegister(3), 0b1100);
}

TEST_F(RiscvCpuTest, EcallInstructionDecodesAndExecutes) {
    uint32_t encoded = AssemblyCompiler::compile("ecall");

    auto inst = InstructionFactory::create(encoded);
    ASSERT_NE(inst, nullptr);
    EXPECT_NO_THROW(inst->execute(*cpu));
}

TEST_F(RiscvCpuTest, EbreakInstructionDecodesAndExecutes) {
    uint32_t encoded = AssemblyCompiler::compile("ebreak");

    auto inst = InstructionFactory::create(encoded);
    ASSERT_NE(inst, nullptr);
    EXPECT_NO_THROW(inst->execute(*cpu));
}

TEST_F(RiscvCpuTest, MretInstructionRestoresProgramCounter) {
    uint32_t targetPc = 0x80000000;
    cpu->setRegister(1, targetPc);

    uint32_t encodedWriteMepc = AssemblyCompiler::compile("csrrw x0, mepc, x1");
    InstructionFactory::create(encodedWriteMepc)->execute(*cpu);

    uint32_t encodedMret = AssemblyCompiler::compile("mret");
    InstructionFactory::create(encodedMret)->execute(*cpu);

    EXPECT_EQ(cpu->getPc(), targetPc);
}

TEST_F(RiscvCpuTest, SretInstructionRestoresProgramCounter) {
    uint32_t targetPc = 0x40000000;

    cpu->setPrivilegeMode(PrivilegeMode::Supervisor);
    cpu->setRegister(1, targetPc);

    uint32_t encodedWriteSepc = AssemblyCompiler::compile("csrrw x0, sepc, x1");
    InstructionFactory::create(encodedWriteSepc)->execute(*cpu);

    uint32_t encodedSret = AssemblyCompiler::compile("sret");
    InstructionFactory::create(encodedSret)->execute(*cpu);

    EXPECT_EQ(cpu->getPc(), targetPc);
}

TEST_F(RiscvCpuTest, SfenceVmaInstructionDecodesAndExecutes) {
    uint32_t encoded = AssemblyCompiler::compile("sfence.vma");

    auto inst = InstructionFactory::create(encoded);
    ASSERT_NE(inst, nullptr);
    EXPECT_NO_THROW(inst->execute(*cpu));
}
