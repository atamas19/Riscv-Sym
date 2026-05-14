#include "RiscvCpuTest.h"

/////////////////////////
// J-Type Instructions //
/////////////////////////

TEST_F(RiscvCpuTest, JalInstructionJumpsForwardAndLinks) {
    uint32_t currentPc = cpu->getPc();

    uint32_t encoded = AssemblyCompiler::compile("jal x1, 256");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(cpu->getRegister(1), currentPc + 4);
    EXPECT_EQ(cpu->getPc(), currentPc + 256);
}

TEST_F(RiscvCpuTest, JalInstructionJumpsBackwardAndLinks) {
    uint32_t currentPc = cpu->getPc();

    uint32_t encoded = AssemblyCompiler::compile("jal x2, -128");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(cpu->getRegister(2), currentPc + 4);
    EXPECT_EQ(cpu->getPc(), currentPc - 128);
}

TEST_F(RiscvCpuTest, JalInstructionWithoutLinking) {
    uint32_t currentPc = cpu->getPc();

    uint32_t encoded = AssemblyCompiler::compile("jal x0, 512");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(cpu->getRegister(0), 0);
    EXPECT_EQ(cpu->getPc(), currentPc + 512);
}
