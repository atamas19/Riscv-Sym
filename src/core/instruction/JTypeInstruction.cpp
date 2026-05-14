#include <core/instruction/JTypeInstruction.h>
#include <core/RiscvCpu.h>
#include <core/Memory.h>

#include <spdlog/fmt/fmt.h>

namespace JType
{

namespace Instruction
{
    namespace {

    int32_t getImm(uint32_t encodedInstruction) {
        int32_t imm = ((getBits(encodedInstruction, 31, 31) << 20)  |
                        (getBits(encodedInstruction, 12, 19) << 12) |
                        (getBits(encodedInstruction, 20, 20) << 11) |
                        (getBits(encodedInstruction, 21, 30) << 1));

        if (getBits(imm, 20, 20) == 1)
            imm |= 0xfff00000;
        else
            imm &= 0x001fffff;

        return imm;
    }

    InstructionArguments getInstructionArguments(uint32_t encodedInstruction) {
        const uint8_t rd = getBits(encodedInstruction, 7, 11);
        const int32_t imm = getImm(encodedInstruction);

        return {imm, rd};
    }

    } // namespace

    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint8_t rd = getBits(encodedInstruction, 7, 11);
        const int32_t imm = getImm(encodedInstruction);

        return JAL::execute({imm, rd}, cpu, instructionOutput);
    }

    bool JAL::execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t targetAddress = cpu.getPc() + instructionArguments.imm;

        if (instructionArguments.rd != 0)
            cpu.setRegister(instructionArguments.rd, cpu.getPc() + 4);

        cpu.setPc(targetAddress);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format("Performed JAL: x{} = PC + 4, PC += {}.", instructionArguments.rd, instructionArguments.imm);
            instructionOutput->setRegisters({instructionArguments.rd});
        }

        return true;
    }

} // namespace Instruction

} // namespace JType
