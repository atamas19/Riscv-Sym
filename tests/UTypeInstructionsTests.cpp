#include "RiscvCpuTest.h"

/////////////////////////
// U-Type Instructions //
/////////////////////////

TEST_F(RiscvCpuTest, LuiInstructionLoadsUpperImmediate) {
    uint32_t encoded = AssemblyCompiler::compile("lui x2, 0x12345");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(cpu->getRegister(2), 0x12345000);
}

TEST_F(RiscvCpuTest, LuiInstructionWithMaxImmediate) {
    uint32_t encoded = AssemblyCompiler::compile("lui x2, 0xFFFFF");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(cpu->getRegister(2), 0xFFFFF000);
}

TEST_F(RiscvCpuTest, AuipcInstructionAddsUpperImmediateToPc) {
    uint32_t currentPc = cpu->getPc();

    uint32_t encoded = AssemblyCompiler::compile("auipc x2, 0x12345");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(cpu->getRegister(2), currentPc + 0x12345000);
}

TEST_F(RiscvCpuTest, AuipcInstructionWithNegativeOffset) {
    uint32_t currentPc = cpu->getPc();

    uint32_t encoded = AssemblyCompiler::compile("auipc x2, 0xFFFFF");
    Instruction::execute(encoded, *cpu);

    EXPECT_EQ(cpu->getRegister(2), currentPc + 0xFFFFF000);
}
