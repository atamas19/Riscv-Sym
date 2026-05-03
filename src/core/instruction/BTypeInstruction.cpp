#include <core/instruction/BTypeInstruction.h>
#include <core/RiscvCpu.h>
#include <core/Memory.h>

#include <unordered_map>
#include <functional>

#include <spdlog/fmt/fmt.h>

namespace BType
{

int32_t Instruction::getImm() {
    int32_t tempValue = ((getBits(instruction, 31, 31) << 12) |
                         (getBits(instruction, 7, 7) << 11)   |
                         (getBits(instruction, 25, 30) << 5)  |
                         (getBits(instruction, 8, 11) << 1));

    if (getBits(tempValue, 12, 12) == 1)
        tempValue |= 0xfffff000;
    else
        tempValue &= 0xfff;

    return tempValue;
}

void Instruction::decode()
{
    rs1 = getBits(instruction, 15, 19);
    rs2 = getBits(instruction, 20, 24);
    imm = getImm();
}

namespace InstructionNew
{
    namespace {
        int32_t getImm(uint32_t instruction) {
            int32_t imm = ( (getBits(instruction, 31, 31) << 12) |
                            (getBits(instruction, 7, 7)   << 11) |
                            (getBits(instruction, 25, 30) << 5)  |
                            (getBits(instruction, 8, 11)  << 1) );

            if (getBits(imm, 12, 12) == 1)
                imm |= 0xfffff000;
            else
                imm &= 0xfff;

            return imm;
        }

        InstructionArguments getInstructionArguments(uint32_t encodedInstruction) {
            const uint8_t rs1 = getBits(encodedInstruction, 15, 19);
            const uint8_t rs2 = getBits(encodedInstruction, 20, 24);
            const int32_t imm = getImm(encodedInstruction);

            return {imm, rs1, rs2};
        }
    } // namespace

    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const InstructionArguments instructionArguments = getInstructionArguments(encodedInstruction);

        const uint8_t funct3 = getBits(encodedInstruction, 12, 14);
        switch (funct3)
        {
        case BEQ::getInstructionDescriptor():
            return BEQ::execute(instructionArguments, cpu, instructionOutput);
        case BNE::getInstructionDescriptor():
            return BNE::execute(instructionArguments, cpu, instructionOutput);
        case BLT::getInstructionDescriptor():
            return BLT::execute(instructionArguments, cpu, instructionOutput);
        case BGE::getInstructionDescriptor():
            return BGE::execute(instructionArguments, cpu, instructionOutput);
        case BLTU::getInstructionDescriptor():
            return BLTU::execute(instructionArguments, cpu, instructionOutput);
        case BGEU::getInstructionDescriptor():
            return BGEU::execute(instructionArguments, cpu, instructionOutput);
        }

        return false;
    }

    namespace BEQ {
        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
            const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
            const uint32_t rs2Value = cpu.getRegister(instructionArguments.rs2);
            const int32_t pcIncrement = (rs1Value == rs2Value) ? instructionArguments.imm : 4;

            cpu.setPc(cpu.getPc() + pcIncrement);

            if (instructionOutput) {
                instructionOutput->consoleLog = fmt::format(
                    "Performed BEQ: x{} ({}) == x{} ({}) -> {}",
                    instructionArguments.rs1, rs1Value, instructionArguments.rs2, rs2Value,
                    (rs1Value == rs2Value) ? fmt::format("branch taken to PC + {}.", instructionArguments.imm) : "branch not taken."
                );
                instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2});
            }

            return true;
        }
    };

    namespace BNE {
        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
            const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
            const uint32_t rs2Value = cpu.getRegister(instructionArguments.rs2);
            const int32_t pcIncrement = (rs1Value != rs2Value) ? instructionArguments.imm : 4;

            cpu.setPc(cpu.getPc() + pcIncrement);

            if (instructionOutput) {
                instructionOutput->consoleLog = fmt::format(
                    "Performed BNE: x{} ({}) != x{} ({}) -> {}",
                    instructionArguments.rs1, rs1Value, instructionArguments.rs2, rs2Value,
                    (rs1Value != rs2Value) ? fmt::format("branch taken to PC + {}.", instructionArguments.imm) : "branch not taken."
                );
                instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2});
            }

            return true;
        }
    };

    namespace BLT {
        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
            const int32_t rs1Value = static_cast<int32_t>(cpu.getRegister(instructionArguments.rs1));
            const int32_t rs2Value = static_cast<int32_t>(cpu.getRegister(instructionArguments.rs2));
            const int32_t pcIncrement = (rs1Value < rs2Value) ? instructionArguments.imm : 4;

            cpu.setPc(cpu.getPc() + pcIncrement);

            if (instructionOutput) {
                instructionOutput->consoleLog = fmt::format(
                    "Performed BLT: x{} ({}) < x{} ({}) -> {}",
                    instructionArguments.rs1, rs1Value, instructionArguments.rs2, rs2Value,
                    (rs1Value < rs2Value) ? fmt::format("branch taken to PC + {}.", instructionArguments.imm) : "branch not taken."
                );
                instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2});
            }
            return true;
        }
    };

    namespace BGE {
        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
            const int32_t rs1Value = static_cast<int32_t>(cpu.getRegister(instructionArguments.rs1));
            const int32_t rs2Value = static_cast<int32_t>(cpu.getRegister(instructionArguments.rs2));
            const int32_t pcIncrement = (rs1Value >= rs2Value) ? instructionArguments.imm : 4;

            cpu.setPc(cpu.getPc() + pcIncrement);

            if (instructionOutput) {
                instructionOutput->consoleLog = fmt::format(
                    "Performed BGE: x{} ({}) >= x{} ({}) -> {}",
                    instructionArguments.rs1, rs1Value, instructionArguments.rs2, rs2Value,
                    (rs1Value >= rs2Value) ? fmt::format("branch taken to PC + {}.", instructionArguments.imm) : "branch not taken."
                );
                instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2});
            }
            return true;
        }
    };

    namespace BLTU {
        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
            const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
            const uint32_t rs2Value = cpu.getRegister(instructionArguments.rs2);
            const int32_t pcIncrement = (rs1Value < rs2Value) ? instructionArguments.imm : 4;

            cpu.setPc(cpu.getPc() + pcIncrement);

            if (instructionOutput) {
                instructionOutput->consoleLog = fmt::format(
                    "Performed BLTU: x{} ({}) < x{} ({}) -> {}",
                    instructionArguments.rs1, rs1Value, instructionArguments.rs2, rs2Value,
                    (rs1Value < rs2Value) ? fmt::format("branch taken to PC + {}.", instructionArguments.imm) : "branch not taken."
                );
                instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2});
            }
            return true;
        }
    };

    namespace BGEU {
        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
            const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
            const uint32_t rs2Value = cpu.getRegister(instructionArguments.rs2);
            const int32_t pcIncrement = (rs1Value >= rs2Value) ? instructionArguments.imm : 4;

            cpu.setPc(cpu.getPc() + pcIncrement);

            if (instructionOutput) {
                instructionOutput->consoleLog = fmt::format(
                    "Performed BGEU: x{} ({}) >= x{} ({}) -> {}",
                    instructionArguments.rs1, rs1Value, instructionArguments.rs2, rs2Value,
                    (rs1Value >= rs2Value) ? fmt::format("branch taken to PC + {}.", instructionArguments.imm) : "branch not taken."
                );
                instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2});
            }
            return true;
        }
    };
};

std::unique_ptr<Instruction> InstructionFactory::create(uint32_t encodedInstruction)
{
    static const std::unordered_map<uint8_t, std::function<std::unique_ptr<Instruction>(uint32_t)>> instructionMap = {
        { BEQ::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<BEQ> (ins); }},
        { BNE::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<BNE> (ins); }},
        { BLT::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<BLT> (ins); }},
        { BGE::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<BGE> (ins); }},
        { BLTU::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<BLTU>(ins); }},
        { BGEU::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<BGEU>(ins); }}
    };

    uint8_t funct3 = getBits(encodedInstruction, 12, 14);

    auto it = instructionMap.find(funct3);
    if (it != instructionMap.end())
        return it->second(encodedInstruction);

    return nullptr;
}

void BEQ::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);

    int32_t pcIncrement = (rs1Value == rs2Value) ? imm : 4;

    cpu.setPc(cpu.getPc() + pcIncrement);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed BEQ: x{} ({}) == x{} ({}) -> {}",
            rs1, rs1Value, rs2, rs2Value,
            (rs1Value == rs2Value) ? fmt::format("branch taken to PC + {}.", imm) : "branch not taken."
        );
        instructionOutput->setRegisters({rs1, rs2});
    }
}

void BNE::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);

    int32_t pcIncrement = (rs1Value != rs2Value) ? imm : 4;

    cpu.setPc(cpu.getPc() + pcIncrement);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed BNE: x{} ({}) != x{} ({}) -> {}",
            rs1, rs1Value, rs2, rs2Value,
            (rs1Value != rs2Value) ? fmt::format("branch taken to PC + {}.", imm) : "branch not taken."
        );
        instructionOutput->setRegisters({rs1, rs2});
    }
}

void BLT::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    int32_t rs1Value = static_cast<int32_t>(cpu.getRegister(rs1));
    int32_t rs2Value = static_cast<int32_t>(cpu.getRegister(rs2));

    int32_t pcIncrement = (rs1Value < rs2Value) ? imm : 4;

    cpu.setPc(cpu.getPc() + pcIncrement);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed BLT: x{} ({}) < x{} ({}) -> {}",
            rs1, rs1Value, rs2, rs2Value,
            (rs1Value < rs2Value) ? fmt::format("branch taken to PC + {}.", imm) : "branch not taken."
        );
        instructionOutput->setRegisters({rs1, rs2});
    }
}

void BGE::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    int32_t rs1Value = static_cast<int32_t>(cpu.getRegister(rs1));
    int32_t rs2Value = static_cast<int32_t>(cpu.getRegister(rs2));

    int32_t pcIncrement = (rs1Value >= rs2Value) ? imm : 4;

    cpu.setPc(cpu.getPc() + pcIncrement);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed BGE: x{} ({}) >= x{} ({}) -> {}",
            rs1, rs1Value, rs2, rs2Value,
            (rs1Value >= rs2Value) ? fmt::format("branch taken to PC + {}.", imm) : "branch not taken."
        );
        instructionOutput->setRegisters({rs1, rs2});
    }
}

void BLTU::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);

    int32_t pcIncrement = (rs1Value < rs2Value) ? imm : 4;

    cpu.setPc(cpu.getPc() + pcIncrement);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed BLTU: x{} ({}) < x{} ({}) -> {}",
            rs1, rs1Value, rs2, rs2Value,
            (rs1Value < rs2Value) ? fmt::format("branch taken to PC + {}.", imm) : "branch not taken."
        );
        instructionOutput->setRegisters({rs1, rs2});
    }
}

void BGEU::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);

    int32_t pcIncrement = (rs1Value >= rs2Value) ? imm : 4;

    cpu.setPc(cpu.getPc() + pcIncrement);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed BGEU: x{} ({}) >= x{} ({}) -> {}",
            rs1, rs1Value, rs2, rs2Value,
            (rs1Value >= rs2Value) ? fmt::format("branch taken to PC + {}.", imm) : "branch not taken."
        );
        instructionOutput->setRegisters({rs1, rs2});
    }
}

} // namespace BType
