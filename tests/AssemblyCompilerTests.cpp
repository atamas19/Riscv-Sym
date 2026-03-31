#include <gtest/gtest.h>

#include <core/AssemblyCompiler.h>

// ==========================================
// Assembly Compiler - CSR Edge Cases
// ==========================================

TEST(AssemblyCompilerCsrTest, ParsesNumericCsrAddress) {
    uint32_t encodedNumeric = AssemblyCompiler::compile("csrrw x0, 0x340, x1");
    uint32_t encodedNamed   = AssemblyCompiler::compile("csrrw x0, mscratch, x1");

    EXPECT_NE(encodedNumeric, 0);
    EXPECT_EQ(encodedNumeric, encodedNamed) << "Numeric address should encode identically to named CSR";
}

TEST(AssemblyCompilerCsrTest, FailsOnUnknownCsrName) {
    uint32_t encoded = AssemblyCompiler::compile("csrrw x0, invalid_csr, x1");
    EXPECT_EQ(encoded, 0) << "Compiler should return 0 for unknown CSR names";
}

TEST(AssemblyCompilerCsrTest, FailsOnOutOfRangeNumericCsr) {
    uint32_t encoded = AssemblyCompiler::compile("csrrw x0, 0x1000, x1");
    EXPECT_EQ(encoded, 0) << "Compiler should return 0 for CSR address > 0xFFF";
}

TEST(AssemblyCompilerCsrTest, ParsesValidCsrImmediate) {
    uint32_t encoded = AssemblyCompiler::compile("csrrwi x0, mscratch, 31");
    EXPECT_NE(encoded, 0) << "Compiler should successfully parse max 5-bit immediate";
}

TEST(AssemblyCompilerCsrTest, FailsOnOutOfRangeCsrImmediate) {
    uint32_t encoded = AssemblyCompiler::compile("csrrwi x0, mscratch, 32");
    EXPECT_EQ(encoded, 0) << "Compiler should return 0 for CSR immediate > 31";
}

TEST(AssemblyCompilerCsrTest, FailsOnNegativeCsrImmediate) {
    uint32_t encoded = AssemblyCompiler::compile("csrrwi x0, mscratch, -1");
    EXPECT_EQ(encoded, 0) << "Compiler should return 0 for negative CSR immediate";
}