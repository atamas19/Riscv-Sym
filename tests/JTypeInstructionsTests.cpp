#include "RiscvCpuTest.h"

/////////////////////////
// J-Type Instructions //
/////////////////////////

TEST_F(RiscvCpuTest, JalInstructionJumpsForwardAndLinks) {
    uint32_t currentPc = cpu->getPc();
    uint32_t expectedReturn = currentPc + 4;

    cpu->setNextPc(expectedReturn);

    uint32_t encoded = AssemblyCompiler::compile("jal x1, 256");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(cpu->getRegister(1), expectedReturn);
    EXPECT_EQ(cpu->getNextPc(), currentPc + 256);
}

TEST_F(RiscvCpuTest, JalInstructionJumpsBackwardAndLinks) {
    uint32_t currentPc = cpu->getPc();
    uint32_t expectedReturn = currentPc + 4;

    cpu->setNextPc(expectedReturn);

    uint32_t encoded = AssemblyCompiler::compile("jal x2, -128");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(cpu->getRegister(2), expectedReturn);
    EXPECT_EQ(cpu->getNextPc(), currentPc - 128);
}

TEST_F(RiscvCpuTest, JalInstructionWithoutLinking) {
    uint32_t currentPc = cpu->getPc();

    cpu->setNextPc(currentPc + 4);

    uint32_t encoded = AssemblyCompiler::compile("jal x0, 512");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(cpu->getRegister(0), 0);
    EXPECT_EQ(cpu->getNextPc(), currentPc + 512);
}