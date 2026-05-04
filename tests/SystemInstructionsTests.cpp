#include "RiscvCpuTest.h"

TEST_F(RiscvCpuTest, CsrRwInstructionWritesAndReads) {
    cpu->setRegister(1, 0x11223344);

    const uint32_t encodedWrite = AssemblyCompiler::compile("csrrw x0, mscratch, x1");
    Instruction_New::execute(encodedWrite, *cpu);

    const uint32_t encodedRead = AssemblyCompiler::compile("csrrw x2, mscratch, x0");
    Instruction_New::execute(encodedRead, *cpu);

    EXPECT_EQ(cpu->getRegister(2), 0x11223344);
}

TEST_F(RiscvCpuTest, CsrRsInstructionSetsBits) {
    cpu->setRegister(1, 0b1010);
    const uint32_t encodedWrite = AssemblyCompiler::compile("csrrw x0, mscratch, x1");
    Instruction_New::execute(encodedWrite, *cpu);

    cpu->setRegister(2, 0b0110);
    const uint32_t encodedSet = AssemblyCompiler::compile("csrrs x3, mscratch, x2");
    Instruction_New::execute(encodedSet, *cpu);

    const uint32_t encodedRead = AssemblyCompiler::compile("csrrw x4, mscratch, x0");
    Instruction_New::execute(encodedRead, *cpu);

    EXPECT_EQ(cpu->getRegister(3), 0b1010);
    EXPECT_EQ(cpu->getRegister(4), 0b1110);
}

TEST_F(RiscvCpuTest, CsrRcInstructionClearsBits) {
    cpu->setRegister(1, 0b1111);
    const uint32_t encodedWrite = AssemblyCompiler::compile("csrrw x0, mscratch, x1");
    Instruction_New::execute(encodedWrite, *cpu);

    cpu->setRegister(2, 0b0101);
    const uint32_t encodedClear = AssemblyCompiler::compile("csrrc x3, mscratch, x2");
    Instruction_New::execute(encodedClear, *cpu);

    const uint32_t encodedRead = AssemblyCompiler::compile("csrrw x4, mscratch, x0");
    Instruction_New::execute(encodedRead, *cpu);

    EXPECT_EQ(cpu->getRegister(3), 0b1111);
    EXPECT_EQ(cpu->getRegister(4), 0b1010);
}

TEST_F(RiscvCpuTest, CsrRwiInstructionWritesImmediate) {
    const uint32_t encodedWriteImm = AssemblyCompiler::compile("csrrwi x0, mscratch, 15");
    Instruction_New::execute(encodedWriteImm, *cpu);

    const uint32_t encodedRead = AssemblyCompiler::compile("csrrw x2, mscratch, x0");
    Instruction_New::execute(encodedRead, *cpu);

    EXPECT_EQ(cpu->getRegister(2), 15);
}

TEST_F(RiscvCpuTest, CsrRsiInstructionSetsBitsImmediate) {
    cpu->setRegister(1, 0b1000);
    const uint32_t encodedWrite = AssemblyCompiler::compile("csrrw x0, mscratch, x1");
    Instruction_New::execute(encodedWrite, *cpu);

    const uint32_t encodedSetImm = AssemblyCompiler::compile("csrrsi x2, mscratch, 7");
    Instruction_New::execute(encodedSetImm, *cpu);

    const uint32_t encodedRead = AssemblyCompiler::compile("csrrw x3, mscratch, x0");
    Instruction_New::execute(encodedRead, *cpu);

    EXPECT_EQ(cpu->getRegister(2), 0b1000);
    EXPECT_EQ(cpu->getRegister(3), 0b1111);
}

TEST_F(RiscvCpuTest, CsrRciInstructionClearsBitsImmediate) {
    cpu->setRegister(1, 0b1111);
    const uint32_t encodedWrite = AssemblyCompiler::compile("csrrw x0, mscratch, x1");
    Instruction_New::execute(encodedWrite, *cpu);

    const uint32_t encodedClearImm = AssemblyCompiler::compile("csrrci x2, mscratch, 3");
    Instruction_New::execute(encodedClearImm, *cpu);

    const uint32_t encodedRead = AssemblyCompiler::compile("csrrw x3, mscratch, x0");
    Instruction_New::execute(encodedRead, *cpu);

    EXPECT_EQ(cpu->getRegister(2), 0b1111);
    EXPECT_EQ(cpu->getRegister(3), 0b1100);
}

TEST_F(RiscvCpuTest, EcallInstructionDecodesAndExecutes) {
    const uint32_t encoded = AssemblyCompiler::compile("ecall");

    const bool inst = Instruction_New::execute(encoded, *cpu);
    EXPECT_TRUE(inst);
}

TEST_F(RiscvCpuTest, EbreakInstructionDecodesAndExecutes) {
    const uint32_t encoded = AssemblyCompiler::compile("ebreak");

    const bool inst = Instruction_New::execute(encoded, *cpu);
    EXPECT_TRUE(inst);
}

TEST_F(RiscvCpuTest, MretInstructionRestoresProgramCounter) {
    const uint32_t targetPc = 0x80000000;
    cpu->setRegister(1, targetPc);

    const uint32_t encodedWriteMepc = AssemblyCompiler::compile("csrrw x0, mepc, x1");
    Instruction_New::execute(encodedWriteMepc, *cpu);

    const uint32_t encodedMret = AssemblyCompiler::compile("mret");
    Instruction_New::execute(encodedMret, *cpu);

    EXPECT_EQ(cpu->getPc(), targetPc);
}

TEST_F(RiscvCpuTest, SretInstructionRestoresProgramCounter) {
    const uint32_t targetPc = 0x40000000;

    cpu->setPrivilegeMode(PrivilegeMode::Supervisor);
    cpu->setRegister(1, targetPc);

    const uint32_t encodedWriteSepc = AssemblyCompiler::compile("csrrw x0, sepc, x1");
    Instruction_New::execute(encodedWriteSepc, *cpu);

    const uint32_t encodedSret = AssemblyCompiler::compile("sret");
    Instruction_New::execute(encodedSret, *cpu);

    EXPECT_EQ(cpu->getPc(), targetPc);
}

TEST_F(RiscvCpuTest, SfenceVmaInstructionDecodesAndExecutes) {
    const uint32_t encoded = AssemblyCompiler::compile("sfence.vma");

    const bool inst = Instruction_New::execute(encoded, *cpu);
    EXPECT_TRUE(inst);
}
