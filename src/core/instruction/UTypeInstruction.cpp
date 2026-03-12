#include <core/instruction/UTypeInstruction.h>
#include <core/RiscvCpu.h>

#include <iostream>

namespace UType
{

void Instruction::decode()
{
    rd  = getBits(instruction, 7, 11);
    imm = (instruction & 0xfffff000);

#if DEBUG
    std::cout << "rd: "   << std::bitset<8>(rd)  << std::endl;
    std::cout << "imm: "  << std::bitset<8>(imm) << std::endl;
#endif
}

void LUI::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    cpu.setRegister(rd, imm);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed LUI: x" + std::to_string(rd) +
                               " = " + std::to_string(imm) + ".";
    instructionOutput.setRegisters({rd});

#if DEBUG
    std::cout << "rdValue: " << std::bitset<32>(cpu.getRegister(rd));
#endif
}

void AUIPC::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput)
{
    uint32_t resultValue = cpu.getPc() + imm;
    uint32_t pc = cpu.getPc();

    cpu.setRegister(rd, resultValue);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "Performed AUIPC: x" + std::to_string(rd) +
                               " = PC (" + std::to_string(pc) + ") + " +
                               std::to_string(imm) + " = " + std::to_string(resultValue) + ".";
    instructionOutput.setRegisters({rd});

#if DEBUG
    std::cout << "rdValue: " << std::bitset<32>(cpu.getRegister(rd));
#endif
}

} // namespace UType
