#include "RiscvCpuTest.h"
#include <core/instruction/CTypeInstruction.h>
#include <core/AssemblyCompiler.h>

//////////////////////////////////////////////////////
// C-Type (Compressed) Instructions - Decompressor  //
//////////////////////////////////////////////////////

// ==========================================
// CADRAN 00 (Opcode 0x0)
// ==========================================

TEST_F(RiscvCpuTest, DecompressC_ADDI4SPN) {
    // C.ADDI4SPN x10, sp, 8
    // RV32C Hex: 0x0028
    uint16_t compressed = 0x0028;
    uint32_t expected = AssemblyCompiler::compile("addi x10, x2, 8");
    EXPECT_EQ(CType::decompress(compressed), expected);
}

TEST_F(RiscvCpuTest, DecompressC_LW) {
    // C.LW x10, 4(x11)
    // RV32C Hex: 0x41C8
    uint16_t compressed = 0x41C8;
    uint32_t expected = AssemblyCompiler::compile("lw x10, 4(x11)");
    EXPECT_EQ(CType::decompress(compressed), expected);
}

TEST_F(RiscvCpuTest, DecompressC_SW) {
    // C.SW x10, 4(x11)
    // RV32C Hex: 0xC1C8
    uint16_t compressed = 0xC1C8;
    uint32_t expected = AssemblyCompiler::compile("sw x10, 4(x11)");
    EXPECT_EQ(CType::decompress(compressed), expected);
}

// ==========================================
// CADRAN 01 (Opcode 0x1)
// ==========================================

TEST_F(RiscvCpuTest, DecompressC_NOP) {
    // C.NOP
    // RV32C Hex: 0x0001
    uint16_t compressed = 0x0001;
    uint32_t expected = AssemblyCompiler::compile("addi x0, x0, 0");
    EXPECT_EQ(CType::decompress(compressed), expected);
}

TEST_F(RiscvCpuTest, DecompressC_ADDI) {
    // C.ADDI x11, 31
    // RV32C Hex: 0x05FD
    uint16_t compressed = 0x05FD;
    uint32_t expected = AssemblyCompiler::compile("addi x11, x11, 31");
    EXPECT_EQ(CType::decompress(compressed), expected);
}

TEST_F(RiscvCpuTest, DecompressC_LI) {
    // C.LI x10, 15
    // RV32C Hex: 0x453D
    uint16_t compressed = 0x453D;
    uint32_t expected = AssemblyCompiler::compile("addi x10, x0, 15");
    EXPECT_EQ(CType::decompress(compressed), expected);
}

TEST_F(RiscvCpuTest, DecompressC_LUI) {
    // C.LUI x10, 0x12
    // RV32C Hex: 0x6549
    uint16_t compressed = 0x6549;
    uint32_t expected = AssemblyCompiler::compile("lui x10, 18"); // 0x12 = 18
    EXPECT_EQ(CType::decompress(compressed), expected);
}

TEST_F(RiscvCpuTest, DecompressC_SRLI) {
    // C.SRLI x10, 4  (x10=rs1'=2)
    // RV32C Hex: 0x8111
    uint16_t compressed = 0x8111;
    uint32_t expected = AssemblyCompiler::compile("srli x10, x10, 4");
    EXPECT_EQ(CType::decompress(compressed), expected);
}

TEST_F(RiscvCpuTest, DecompressC_SRAI) {
    // C.SRAI x10, 4
    // RV32C Hex: 0x8511
    uint16_t compressed = 0x8511;
    uint32_t expected = AssemblyCompiler::compile("srai x10, x10, 4");
    EXPECT_EQ(CType::decompress(compressed), expected);
}

TEST_F(RiscvCpuTest, DecompressC_ANDI) {
    // C.ANDI x10, 15
    // RV32C Hex: 0x893D
    uint16_t compressed = 0x893D;
    uint32_t expected = AssemblyCompiler::compile("andi x10, x10, 15");
    EXPECT_EQ(CType::decompress(compressed), expected);
}

TEST_F(RiscvCpuTest, DecompressC_SUB) {
    // C.SUB x10, x11  (rs1'=2, rs2'=3)
    // RV32C Hex: 0x8D0D
    uint16_t compressed = 0x8D0D;
    uint32_t expected = AssemblyCompiler::compile("sub x10, x10, x11");
    EXPECT_EQ(CType::decompress(compressed), expected);
}

TEST_F(RiscvCpuTest, DecompressC_XOR) {
    // C.XOR x10, x11
    // RV32C Hex: 0x8D2D
    uint16_t compressed = 0x8D2D;
    uint32_t expected = AssemblyCompiler::compile("xor x10, x10, x11");
    EXPECT_EQ(CType::decompress(compressed), expected);
}

TEST_F(RiscvCpuTest, DecompressC_OR) {
    // C.OR x10, x11
    // RV32C Hex: 0x8D4D
    uint16_t compressed = 0x8D4D;
    uint32_t expected = AssemblyCompiler::compile("or x10, x10, x11");
    EXPECT_EQ(CType::decompress(compressed), expected);
}

TEST_F(RiscvCpuTest, DecompressC_AND) {
    // C.AND x10, x11
    // RV32C Hex: 0x8D6D
    uint16_t compressed = 0x8D6D;
    uint32_t expected = AssemblyCompiler::compile("and x10, x10, x11");
    EXPECT_EQ(CType::decompress(compressed), expected);
}

// ==========================================
// CADRAN 10 (Opcode 0x2)
// ==========================================

TEST_F(RiscvCpuTest, DecompressC_SLLI) {
    // C.SLLI x10, 4
    // RV32C Hex: 0x0512
    uint16_t compressed = 0x0512;
    uint32_t expected = AssemblyCompiler::compile("slli x10, x10, 4");
    EXPECT_EQ(CType::decompress(compressed), expected);
}

TEST_F(RiscvCpuTest, DecompressC_LWSP) {
    // C.LWSP x10, 8(sp)
    // RV32C Hex: 0x4522
    uint16_t compressed = 0x4522;
    uint32_t expected = AssemblyCompiler::compile("lw x10, 8(x2)");
    EXPECT_EQ(CType::decompress(compressed), expected);
}

TEST_F(RiscvCpuTest, DecompressC_SWSP) {
    // C.SWSP x10, 8(sp)
    // RV32C Hex: 0xC42A
    uint16_t compressed = 0xC42A;
    uint32_t expected = AssemblyCompiler::compile("sw x10, 8(x2)");
    EXPECT_EQ(CType::decompress(compressed), expected);
}

TEST_F(RiscvCpuTest, DecompressC_JR) {
    // C.JR x10
    // RV32C Hex: 0x8502
    uint16_t compressed = 0x8502;
    uint32_t expected = AssemblyCompiler::compile("jalr x0, 0(x10)");
    EXPECT_EQ(CType::decompress(compressed), expected);
}

TEST_F(RiscvCpuTest, DecompressC_JALR) {
    // C.JALR x10
    // RV32C Hex: 0x9502
    uint16_t compressed = 0x9502;
    uint32_t expected = AssemblyCompiler::compile("jalr x1, 0(x10)");
    EXPECT_EQ(CType::decompress(compressed), expected);
}

TEST_F(RiscvCpuTest, DecompressC_MV) {
    // C.MV x10, x11
    // RV32C Hex: 0x852E
    uint16_t compressed = 0x852E;
    uint32_t expected = AssemblyCompiler::compile("add x10, x0, x11");
    EXPECT_EQ(CType::decompress(compressed), expected);
}

TEST_F(RiscvCpuTest, DecompressC_ADD) {
    // C.ADD x10, x11
    // RV32C Hex: 0x952E
    uint16_t compressed = 0x952E;
    uint32_t expected = AssemblyCompiler::compile("add x10, x10, x11");
    EXPECT_EQ(CType::decompress(compressed), expected);
}

TEST_F(RiscvCpuTest, DecompressC_EBREAK) {
    // C.EBREAK
    // RV32C Hex: 0x9002
    uint16_t compressed = 0x9002;
    uint32_t expected = AssemblyCompiler::compile("ebreak");
    EXPECT_EQ(CType::decompress(compressed), expected);
}

TEST_F(RiscvCpuTest, IllegalCompressedInstructionReturnsZero) {
    uint16_t illegal = 0x0000;
    EXPECT_EQ(CType::decompress(illegal), 0);
}

TEST_F(RiscvCpuTest, ExecuteEndToEndCompressed_ADD) {
    cpu->setRegister(10, 50); // x10
    cpu->setRegister(11, 25); // x11

    // C.ADD x10, x11 (0x952E)
    uint16_t compressed = 0x952E;
    uint32_t decompressed = CType::decompress(compressed);

    Instruction::execute(decompressed, *cpu);

    EXPECT_EQ(cpu->getRegister(10), 75);
}

TEST_F(RiscvCpuTest, ExecuteEndToEndCompressed_LWSP) {
    // C.LWSP x10, 8(sp)

    cpu->setRegister(2, 0x1000);

    Memory::getInstance().write32(0x1008, 0xDEADBEEF);

    uint16_t compressed = 0x4522; // C.LWSP x10, 8(sp)
    uint32_t decompressed = CType::decompress(compressed);

    Instruction::execute(decompressed, *cpu);

    EXPECT_EQ(cpu->getRegister(10), 0xDEADBEEF);
}
