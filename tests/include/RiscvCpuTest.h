#pragma once

#include <gtest/gtest.h>
#include <core/RiscvCpu.h>
#include <core/Memory.h>
#include <core/AssemblyCompiler.h>
#include <core/instruction/Instruction.h>

class RiscvCpuTest : public ::testing::Test {
protected:
    RiscvCpu* cpu;

    void SetUp() override {
        cpu = &RiscvCpu::getInstance();
        cpu->reset();
        cpu->setPc(0x1000);
        for (int i = 0; i < 32; i++) {
            cpu->setRegister(i, 0);
        }

        Memory::getInstance().reset();
    }

    void TearDown() override {

    }
};
