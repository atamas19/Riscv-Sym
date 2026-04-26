#include <core/instruction/RTypeInstruction.h>
#include <core/RiscvCpu.h>

#include <unordered_map>
#include <limits>

#include <spdlog/fmt/fmt.h>

namespace RType
{

void Instruction::decode()
{
    rd  = getBits(instruction, 7, 11);
    rs1 = getBits(instruction, 15, 19);
    rs2 = getBits(instruction, 20, 24);
}

static inline const uint16_t createRuntimeInstructionDescription(uint8_t funct3, uint8_t funct7) {
    return (static_cast<uint16_t>(funct3) << 8) | funct7;
}

namespace InstructionNew
{
    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint8_t rd  = getBits(encodedInstruction, 7, 11);
        const uint8_t rs1 = getBits(encodedInstruction, 15, 19);
        const uint8_t rs2 = getBits(encodedInstruction, 20, 24);

        const uint16_t funct3 = getBits(encodedInstruction, 12, 14);
        const uint8_t funct7 = getBits(encodedInstruction, 25, 31);
        const uint16_t instructionDescriptor = createRuntimeInstructionDescription(funct3, funct7);
        switch (instructionDescriptor)
        {
        case ADD::getInstructionDescription():
            return ADD::execute({rs1, rs2, rd}, cpu, instructionOutput);
        case SUB::getInstructionDescription():
            return SUB::execute({rs1, rs2, rd}, cpu, instructionOutput);
        case SLL::getInstructionDescription():
            return SLL::execute({rs1, rs2, rd}, cpu, instructionOutput);
        case SLT::getInstructionDescription():
            return SLT::execute({rs1, rs2, rd}, cpu, instructionOutput);
        case SLTU::getInstructionDescription():
            return SLTU::execute({rs1, rs2, rd}, cpu, instructionOutput);
        case XOR::getInstructionDescription():
            return XOR::execute({rs1, rs2, rd}, cpu, instructionOutput);
        case SRL::getInstructionDescription():
            return SRL::execute({rs1, rs2, rd}, cpu, instructionOutput);
        case SRA::getInstructionDescription():
            return SRA::execute({rs1, rs2, rd}, cpu, instructionOutput);
        case OR::getInstructionDescription():
            return OR::execute({rs1, rs2, rd}, cpu, instructionOutput);
        case AND::getInstructionDescription():
            return AND::execute({rs1, rs2, rd}, cpu, instructionOutput);
        case MUL::getInstructionDescription():
            return MUL::execute({rs1, rs2, rd}, cpu, instructionOutput);
        case MULH::getInstructionDescription():
            return MULH::execute({rs1, rs2, rd}, cpu, instructionOutput);
        case MULHSU::getInstructionDescription():
            return MULHSU::execute({rs1, rs2, rd}, cpu, instructionOutput);
        case MULHU::getInstructionDescription():
            return MULHU::execute({rs1, rs2, rd}, cpu, instructionOutput);
        case DIV::getInstructionDescription():
            return DIV::execute({rs1, rs2, rd}, cpu, instructionOutput);
        case DIVU::getInstructionDescription():
            return DIVU::execute({rs1, rs2, rd}, cpu, instructionOutput);
        case REM::getInstructionDescription():
            return REM::execute({rs1, rs2, rd}, cpu, instructionOutput);
        case REMU::getInstructionDescription():
            return REMU::execute({rs1, rs2, rd}, cpu, instructionOutput);
        }

        return false;
    }

    bool ADD::execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
        const uint32_t rs2Value = cpu.getRegister(instructionArguments.rs2);
        const uint32_t result = rs2Value + rs1Value;

        cpu.setRegister(instructionArguments.rd, result);
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed ADD: x{} = x{} ({}) + x{} ({}).",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, instructionArguments.rs2, rs2Value
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2, instructionArguments.rd});
        }

        return true;
    }

    bool SUB::execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
        const uint32_t rs2Value = cpu.getRegister(instructionArguments.rs2);
        const uint32_t result = rs1Value - rs2Value;

        cpu.setRegister(instructionArguments.rd, result);
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed SUB: x{} = x{} ({}) - x{} ({}).",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, instructionArguments.rs2, rs2Value
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2, instructionArguments.rd});
        }

        return true;
    }

    bool SLL::execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
        const uint32_t rs2Value = cpu.getRegister(instructionArguments.rs2);
        const uint8_t shiftValue = getBits(rs2Value, 0, 5);
        const uint32_t result = rs1Value << shiftValue;

        cpu.setRegister(instructionArguments.rd, result);
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed SLL: x{} = x{} ({}) << x{} ({} bits).",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, instructionArguments.rs2, rs2Value
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2, instructionArguments.rd});
        }

        return true;
    }

    bool SLT::execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const int32_t rs1Value = static_cast<int32_t>(cpu.getRegister(instructionArguments.rs1));
        const int32_t rs2Value = static_cast<int32_t>(cpu.getRegister(instructionArguments.rs2));
        const uint8_t result = (rs1Value < rs2Value);

        cpu.setRegister(instructionArguments.rd, result);
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed SLT: x{} = (x{} ({}) < x{} ({})) → {}.",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, instructionArguments.rs2, rs2Value, result
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2, instructionArguments.rd});
        }

        return true;
    }

    bool SLTU::execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
        const uint32_t rs2Value = cpu.getRegister(instructionArguments.rs2);
        const uint8_t result = (rs1Value < rs2Value);

        cpu.setRegister(instructionArguments.rd, result);
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed SLTU: x{} = (unsigned)x{} ({}) < (unsigned)x{} ({}) → {}.",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, instructionArguments.rs2, rs2Value, result
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2, instructionArguments.rd});
        }

        return true;
    }

    bool XOR::execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
        const uint32_t rs2Value = cpu.getRegister(instructionArguments.rs2);
        const uint32_t result = (rs1Value ^ rs2Value);

        cpu.setRegister(instructionArguments.rd, result);
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed XOR: x{} = x{} ({}) ^ x{} ({}).",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, instructionArguments.rs2, rs2Value
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2, instructionArguments.rd});
        }

        return true;
    }

    bool SRL::execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
        const uint32_t rs2Value = cpu.getRegister(instructionArguments.rs2);
        const uint8_t shiftValue = getBits(rs2Value, 0, 5);
        const uint32_t result = static_cast<uint32_t>(rs1Value) >> shiftValue;

        cpu.setRegister(instructionArguments.rd, result);
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed SRL: x{} = x{} ({}) >> x{} ({} bits) (logical shift).",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, instructionArguments.rs2, shiftValue
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2, instructionArguments.rd});
        }

        return true;
    }

    bool SRA::execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
        const uint32_t rs2Value = cpu.getRegister(instructionArguments.rs2);
        const uint8_t shiftValue = getBits(rs2Value, 0, 5);
        const uint32_t result = static_cast<uint32_t>(static_cast<int32_t>(rs1Value) >> shiftValue);

        cpu.setRegister(instructionArguments.rd, result);
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed SRA: x{} = x{} ({}) >> x{} ({} bits) (arithmetic shift).",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, instructionArguments.rs2, shiftValue
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2, instructionArguments.rd});
        }

        return true;
    }

    bool OR::execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
        const uint32_t rs2Value = cpu.getRegister(instructionArguments.rs2);
        const uint32_t result = (rs1Value | rs2Value);

        cpu.setRegister(instructionArguments.rd, result);
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed OR: x{} = x{} ({}) | x{} ({}).",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, instructionArguments.rs2, rs2Value
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2, instructionArguments.rd});
        }

        return true;
    }

    bool AND::execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
        const uint32_t rs2Value = cpu.getRegister(instructionArguments.rs2);
        const uint32_t result = (rs1Value & rs2Value);

        cpu.setRegister(instructionArguments.rd, result);
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed AND: x{} = x{} ({}) & x{} ({}).",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, instructionArguments.rs2, rs2Value
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2, instructionArguments.rd});
        }

        return true;
    }

    // RV32M instructions

    bool MUL::execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
        const uint32_t rs2Value = cpu.getRegister(instructionArguments.rs2);
        const uint32_t result = rs1Value * rs2Value;

        cpu.setRegister(instructionArguments.rd, result);
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed MUL: x{} = x{} ({}) * x{} ({})",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, instructionArguments.rs2, rs2Value
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2, instructionArguments.rd});
        }

        return true;
    }

    bool MULH::execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const int64_t rs1Value = static_cast<int32_t>(cpu.getRegister(instructionArguments.rs1));
        const int64_t rs2Value = static_cast<int32_t>(cpu.getRegister(instructionArguments.rs2));
        const int64_t result = rs1Value * rs2Value;

        cpu.setRegister(instructionArguments.rd, static_cast<uint32_t>(static_cast<uint64_t>(result) >> 32));
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed MULH: x{} = x{} ({}) * x{} ({}) [Upper 32 bits].",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, instructionArguments.rs2, rs2Value
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2, instructionArguments.rd});
        }

        return true;
    }

    bool MULHSU::execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const int64_t rs1Value = static_cast<int32_t>(cpu.getRegister(instructionArguments.rs1));
        const int64_t rs2Value = static_cast<int64_t>(cpu.getRegister(instructionArguments.rs2));
        const int64_t result = rs1Value * rs2Value;

        cpu.setRegister(instructionArguments.rd, static_cast<uint32_t>(static_cast<uint64_t>(result) >> 32));
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed MULHSU: x{} = x{} ({}) * x{} ({}) [Upper 32 bits].",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, instructionArguments.rs2, rs2Value
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2, instructionArguments.rd});
        }

        return true;
    }

    bool MULHU::execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint64_t rs1Value = cpu.getRegister(instructionArguments.rs1);
        const uint64_t rs2Value = cpu.getRegister(instructionArguments.rs2);
        const uint64_t result = rs1Value * rs2Value;

        cpu.setRegister(instructionArguments.rd, static_cast<uint32_t>(result >> 32));
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed MULHU: x{} = x{} ({}) * x{} ({}) [Upper 32 bits].",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, instructionArguments.rs2, rs2Value
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2, instructionArguments.rd});
        }

        return true;
    }

    bool DIV::execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const int32_t rs1Value = static_cast<int32_t>(cpu.getRegister(instructionArguments.rs1));
        const int32_t rs2Value = static_cast<int32_t>(cpu.getRegister(instructionArguments.rs2));
        uint32_t result;

        if (rs2Value == 0) {
            result = 0xFFFFFFFF;
        } else if (rs1Value == std::numeric_limits<int32_t>::min() && rs2Value == -1) {
            result = static_cast<uint32_t>(rs1Value);
        } else {
            result = static_cast<uint32_t>(rs1Value / rs2Value);
        }

        cpu.setRegister(instructionArguments.rd, result);
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed DIV: x{} = x{} ({}) / x{} ({})",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, instructionArguments.rs2, rs2Value
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2, instructionArguments.rd});
        }

        return true;
    }

    bool DIVU::execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
        const uint32_t rs2Value = cpu.getRegister(instructionArguments.rs2);
        uint32_t result;

        if (rs2Value == 0) {
            result = 0xFFFFFFFF;
        } else {
            result = rs1Value / rs2Value;
        }

        cpu.setRegister(instructionArguments.rd, result);
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed DIVU: x{} = x{} ({}) / x{} ({})",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, instructionArguments.rs2, rs2Value
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2, instructionArguments.rd});
        }

        return true;
    }

    bool REM::execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const int32_t rs1Value = static_cast<int32_t>(cpu.getRegister(instructionArguments.rs1));
        const int32_t rs2Value = static_cast<int32_t>(cpu.getRegister(instructionArguments.rs2));
        uint32_t result;

        if (rs2Value == 0) {
            result = static_cast<uint32_t>(rs1Value);
        } else if (rs1Value == std::numeric_limits<int32_t>::min() && rs2Value == -1) {
            result = 0;
        } else {
            result = static_cast<uint32_t>(rs1Value % rs2Value);
        }

        cpu.setRegister(instructionArguments.rd, result);
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed REM: x{} = x{} ({}) % x{} ({})",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, instructionArguments.rs2, rs2Value
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2, instructionArguments.rd});
        }

        return true;
    }

    bool REMU::execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t rs1Value = cpu.getRegister(instructionArguments.rs1);
        const uint32_t rs2Value = cpu.getRegister(instructionArguments.rs2);
        uint32_t result;

        if (rs2Value == 0) {
            result = rs1Value;
        } else {
            result = rs1Value % rs2Value;
        }

        cpu.setRegister(instructionArguments.rd, result);
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed REMU: x{} = x{} ({}) % x{} ({})",
                instructionArguments.rd, instructionArguments.rs1, rs1Value, instructionArguments.rs2, rs2Value
            );
            instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2, instructionArguments.rd});
        }

        return true;
    }

};

namespace AtomicNew
{
    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint8_t rd  = getBits(encodedInstruction, 7, 11);
        const uint8_t rs1 = getBits(encodedInstruction, 15, 19);
        const uint8_t rs2 = getBits(encodedInstruction, 20, 24);

        const uint8_t funct3 = getBits(encodedInstruction, 12, 14);
        const uint8_t funct7 = getBits(encodedInstruction, 25, 31);

        uint8_t amo_op = funct7 >> 2;

        const uint16_t instructionDescriptor = createRuntimeInstructionDescription(funct3, amo_op);
        switch (instructionDescriptor) {
            case LR::getInstructionDescription(): return LR::execute({rs1, rs2, rd}, cpu, instructionOutput);
            case SC::getInstructionDescription(): return SC::execute({rs1, rs2, rd}, cpu, instructionOutput);

            case AMO::ADD:: getInstructionDescription(): return AMO::ADD:: execute({rs1, rs2, rd}, cpu, instructionOutput);
            case AMO::SWAP::getInstructionDescription(): return AMO::SWAP::execute({rs1, rs2, rd}, cpu, instructionOutput);
            case AMO::XOR:: getInstructionDescription(): return AMO::XOR:: execute({rs1, rs2, rd}, cpu, instructionOutput);
            case AMO::OR::  getInstructionDescription(): return AMO::OR::  execute({rs1, rs2, rd}, cpu, instructionOutput);
            case AMO::AND:: getInstructionDescription(): return AMO::AND:: execute({rs1, rs2, rd}, cpu, instructionOutput);
            case AMO::MIN:: getInstructionDescription(): return AMO::MIN:: execute({rs1, rs2, rd}, cpu, instructionOutput);
            case AMO::MAX:: getInstructionDescription(): return AMO::MAX:: execute({rs1, rs2, rd}, cpu, instructionOutput);
            case AMO::MINU::getInstructionDescription(): return AMO::MINU::execute({rs1, rs2, rd}, cpu, instructionOutput);
            case AMO::MAXU::getInstructionDescription(): return AMO::MAXU::execute({rs1, rs2, rd}, cpu, instructionOutput);
            default: return false;
        }
    }

    namespace LR {
        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
            Memory& mem = Memory::getInstance();
            const uint32_t virtual_address = cpu.getRegister(instructionArguments.rs1);
            const uint32_t value = mem.read32(virtual_address);

            const uint32_t physical_address = mem.translateAddress(virtual_address, AccessType::Load);

            cpu.makeReservation(physical_address);

            if (instructionArguments.rd != 0) {
                cpu.setRegister(instructionArguments.rd, value);
            }

            cpu.setPc(cpu.getPc() + 4);

            if (instructionOutput) {
                instructionOutput->consoleLog = fmt::format(
                    "Performed LR.W: x{} <- Mem[0x{:X}], Reservation Set", instructionArguments.rd, virtual_address
                );
                instructionOutput->modifiedRamAddresses.push_back({virtual_address, value});
                instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rd});
            }

            return true;
        }
    };

    namespace SC {
        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
            const uint32_t virtual_address = cpu.getRegister(instructionArguments.rs1);
            Memory& mem = Memory::getInstance();

            const uint32_t physical_address = mem.translateAddress(virtual_address, AccessType::Load);

            if (cpu.checkAndClearReservation(physical_address)) {
                const uint32_t value_to_write = cpu.getRegister(instructionArguments.rs2);
                mem.write32(virtual_address, value_to_write);
                if (instructionArguments.rd != 0) cpu.setRegister(instructionArguments.rd, 0);

                if (instructionOutput) {
                    instructionOutput->consoleLog = fmt::format(
                        "Performed SC.W: SUCCESS, Mem[0x{:X}] <- {} (x{}), x{} <- 0",
                        virtual_address, value_to_write, instructionArguments.rs2, instructionArguments.rd
                    );
                    instructionOutput->modifiedRamAddresses.push_back({virtual_address, value_to_write});
                    instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2, instructionArguments.rd});
                }
            } else {
                if (instructionArguments.rd != 0) cpu.setRegister(instructionArguments.rd, 1);

                if (instructionOutput) {
                    instructionOutput->consoleLog = fmt::format(
                        "Performed SC.W: FAILED (Reservation lost or mismatch), x{} <- 1", instructionArguments.rd
                    );
                    instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rd});
                }
            }

            cpu.setPc(cpu.getPc() + 4);

            return true;
        }
    };

    namespace AMO
    {
        bool execute(InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
            Memory& mem = Memory::getInstance();

            const uint32_t memory_address = cpu.getRegister(instructionArguments.rs1);
            const uint32_t value_from_rs2 = cpu.getRegister(instructionArguments.rs2);
            const uint32_t old_value = mem.read32(memory_address);
            const uint32_t new_value = instructionArguments.performAmoOperation(old_value, value_from_rs2);

            cpu.notifyStore(memory_address, 4);
            mem.write32(memory_address, new_value);

            if (instructionArguments.rd != 0) {
                cpu.setRegister(instructionArguments.rd, old_value);
            }

            cpu.setPc(cpu.getPc() + 4);

            if (instructionOutput) {
                instructionOutput->consoleLog = fmt::format(
                    "Performed {}.W: Mem[0x{:X}] <- {} (old: {}, rs2: {}), old_val -> x{}",
                    instructionArguments.instructionName, memory_address, new_value, old_value, value_from_rs2, instructionArguments.rd
                );
                instructionOutput->modifiedRamAddresses.push_back({memory_address, new_value});
                instructionOutput->setRegisters({instructionArguments.rs1, instructionArguments.rs2, instructionArguments.rd});
            }

            return true;
        }

        namespace SWAP {
            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
                auto swapFunction = [](uint32_t old_val, uint32_t rs2_val) { return rs2_val; };
                return AMO::execute({instructionArguments, "AMOSWAP", swapFunction}, cpu, instructionOutput);
            }
        };

        namespace ADD {
            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
                auto addFunction = [](uint32_t old_val, uint32_t rs2_val) { return old_val + rs2_val; };
                return AMO::execute({instructionArguments, "AMOADD", addFunction}, cpu, instructionOutput);
            }
        };

        namespace XOR {
            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
                auto xorFunction = [](uint32_t old_val, uint32_t rs2_val) { return old_val ^ rs2_val; };
                return AMO::execute({instructionArguments, "AMOXOR", xorFunction}, cpu, instructionOutput);
            }
        };

        namespace AND {
            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
                auto andFunction = [](uint32_t old_val, uint32_t rs2_val) { return old_val & rs2_val; };
                return AMO::execute({instructionArguments, "AMOAND", andFunction}, cpu, instructionOutput);
            }
        };

        namespace OR {
            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
                auto orFunction = [](uint32_t old_val, uint32_t rs2_val) { return old_val | rs2_val; };
                return AMO::execute({instructionArguments, "AMOOR", orFunction}, cpu, instructionOutput);
            }
        };

        namespace MIN {
            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
                auto minFunction = [](uint32_t old_val, uint32_t rs2_val) {
                    int32_t signed_old = static_cast<int32_t>(old_val);
                    int32_t signed_rs2 = static_cast<int32_t>(rs2_val);
                    return static_cast<uint32_t>(std::min(signed_old, signed_rs2));
                };
                return AMO::execute({instructionArguments, "AMOMIN", minFunction}, cpu, instructionOutput);
            }
        };

        namespace MAX {
            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
                auto maxFunction = [](uint32_t old_val, uint32_t rs2_val) {
                    int32_t signed_old = static_cast<int32_t>(old_val);
                    int32_t signed_rs2 = static_cast<int32_t>(rs2_val);
                    return static_cast<uint32_t>(std::max(signed_old, signed_rs2));
                };
                return AMO::execute({instructionArguments, "AMOMAX", maxFunction}, cpu, instructionOutput);
            }
        };

        namespace MINU {
            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
                auto minuFunction = [](uint32_t old_val, uint32_t rs2_val) {
                    return std::min(old_val, rs2_val);
                };
                return AMO::execute({instructionArguments, "AMOMINU", minuFunction}, cpu, instructionOutput);
            }
        };

        namespace MAXU {
            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
                auto maxuFunction = [](uint32_t old_val, uint32_t rs2_val) {
                    return std::max(old_val, rs2_val);
                };
                return AMO::execute({instructionArguments, "AMOMAXU", maxuFunction}, cpu, instructionOutput);
            }
        };
    };
};

std::unique_ptr<Instruction> InstructionFactory::create(uint32_t encodedInstruction)
{
    static const std::unordered_map<InstructionDescriptor, std::function<std::unique_ptr<Instruction>(uint32_t)>, InstructionDescriptor::InstructionDescriptorHash> instructionMap = {
        { SLTU::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<SLTU>(ins); }},
        { ADD::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<ADD> (ins); }},
        { SUB::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<SUB> (ins); }},
        { SLL::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<SLL> (ins); }},
        { SLT::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<SLT> (ins); }},
        { XOR::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<XOR> (ins); }},
        { SRL::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<SRL> (ins); }},
        { SRA::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<SRA> (ins); }},
        { AND::getInstructionDescriptor (), [](uint32_t ins) { return std::make_unique<AND> (ins); }},
        { OR::getInstructionDescriptor  (), [](uint32_t ins) { return std::make_unique<OR>  (ins); }},
        // RV32M instructions
        {    MUL::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<MUL>   (ins); }},
        {   MULH::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<MULH>  (ins); }},
        { MULHSU::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<MULHSU>(ins); }},
        {  MULHU::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<MULHU> (ins); }},
        {    DIV::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<DIV>   (ins); }},
        {   DIVU::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<DIVU>  (ins); }},
        {    REM::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<REM>   (ins); }},
        {   REMU::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<REMU>  (ins); }}
    };
    uint8_t funct3 = getBits(encodedInstruction, 12, 14);
    uint8_t funct7 = getBits(encodedInstruction, 25, 31);

    InstructionDescriptor descriptor{funct3, funct7};

    auto it = instructionMap.find(descriptor);
    if (it != instructionMap.end())
        return it->second(encodedInstruction);

    return nullptr;
}

void ADD::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);

    uint32_t result = rs2Value + rs1Value;

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed ADD: x{} = x{} ({}) + x{} ({}).",
            rd, rs1, rs1Value, rs2, rs2Value
        );
        instructionOutput->setRegisters({rs1, rs2, rd});
    }
}

void SUB::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);

    uint32_t result = rs1Value - rs2Value;

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed SUB: x{} = x{} ({}) - x{} ({}).",
            rd, rs1, rs1Value, rs2, rs2Value
        );
        instructionOutput->setRegisters({rs1, rs2, rd});
    }
}

void SLL::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);

    uint8_t shiftValue = getBits(rs2Value, 0, 5);

    uint32_t result = rs1Value << shiftValue;

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed SLL: x{} = x{} ({}) << x{} ({} bits).",
            rd, rs1, rs1Value, rs2, rs2Value
        );
        instructionOutput->setRegisters({rs1, rs2, rd});
    }
}

void SLT::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    int32_t rs1Value = static_cast<int32_t>(cpu.getRegister(rs1));
    int32_t rs2Value = static_cast<int32_t>(cpu.getRegister(rs2));

    uint8_t result = (rs1Value < rs2Value);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed SLT: x{} = (x{} ({}) < x{} ({})) → {}.",
            rd, rs1, rs1Value, rs2, rs2Value, result
        );
        instructionOutput->setRegisters({rs1, rs2, rd});
    }
}

void SLTU::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);

    uint8_t result = (rs1Value < rs2Value);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed SLTU: x{} = (unsigned)x{} ({}) < (unsigned)x{} ({}) → {}.",
            rd, rs1, rs1Value, rs2, rs2Value, result
        );
        instructionOutput->setRegisters({rs1, rs2, rd});
    }
}

void XOR::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);

    uint32_t result = (rs1Value ^ rs2Value);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed XOR: x{} = x{} ({}) ^ x{} ({}).",
            rd, rs1, rs1Value, rs2, rs2Value
        );
        instructionOutput->setRegisters({rs1, rs2, rd});
    }
}

void SRL::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);

    uint8_t shiftValue = getBits(rs2Value, 0, 5);

    uint32_t result = static_cast<uint32_t>(rs1Value) >> shiftValue;

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed SRL: x{} = x{} ({}) >> x{} ({} bits) (logical shift).",
            rd, rs1, rs1Value, rs2, shiftValue
        );
        instructionOutput->setRegisters({rs1, rs2, rd});
    }
}

void SRA::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);

    uint8_t shiftValue = getBits(rs2Value, 0, 5);

    uint32_t result = static_cast<uint32_t>(static_cast<int32_t>(rs1Value) >> shiftValue);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed SRA: x{} = x{} ({}) >> x{} ({} bits) (arithmetic shift).",
            rd, rs1, rs1Value, rs2, shiftValue
        );
        instructionOutput->setRegisters({rs1, rs2, rd});
    }
}

void OR::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);

    uint32_t result = (rs1Value | rs2Value);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed OR: x{} = x{} ({}) | x{} ({}).",
            rd, rs1, rs1Value, rs2, rs2Value
        );
        instructionOutput->setRegisters({rs1, rs2, rd});
    }
}

void AND::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);

    uint32_t result = (rs1Value & rs2Value);

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed AND: x{} = x{} ({}) & x{} ({}).",
            rd, rs1, rs1Value, rs2, rs2Value
        );
        instructionOutput->setRegisters({rs1, rs2, rd});
    }
}

// RV32M instructions

void MUL::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);

    uint32_t result = rs1Value * rs2Value;

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed MUL: x{} = x{} ({}) * x{} ({})",
            rd, rs1, rs1Value, rs2, rs2Value
        );
        instructionOutput->setRegisters({rs1, rs2, rd});
    }
}

void MULH::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    int64_t rs1Value = static_cast<int32_t>(cpu.getRegister(rs1));
    int64_t rs2Value = static_cast<int32_t>(cpu.getRegister(rs2));

    int64_t result = rs1Value * rs2Value;

    cpu.setRegister(rd, static_cast<uint32_t>(static_cast<uint64_t>(result) >> 32));
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed MULH: x{} = x{} ({}) * x{} ({}) [Upper 32 bits].",
            rd, rs1, rs1Value, rs2, rs2Value
        );
        instructionOutput->setRegisters({rs1, rs2, rd});
    }
}

void MULHSU::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    int64_t rs1Value = static_cast<int32_t>(cpu.getRegister(rs1));
    int64_t rs2Value = static_cast<int64_t>(cpu.getRegister(rs2));

    int64_t result = rs1Value * rs2Value;

    cpu.setRegister(rd, static_cast<uint32_t>(static_cast<uint64_t>(result) >> 32));
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed MULHSU: x{} = x{} ({}) * x{} ({}) [Upper 32 bits].",
            rd, rs1, rs1Value, rs2, rs2Value
        );
        instructionOutput->setRegisters({rs1, rs2, rd});
    }
}

void MULHU::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint64_t rs1Value = cpu.getRegister(rs1);
    uint64_t rs2Value = cpu.getRegister(rs2);

    uint64_t result = rs1Value * rs2Value;

    cpu.setRegister(rd, static_cast<uint32_t>(result >> 32));
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed MULHU: x{} = x{} ({}) * x{} ({}) [Upper 32 bits].",
            rd, rs1, rs1Value, rs2, rs2Value
        );
        instructionOutput->setRegisters({rs1, rs2, rd});
    }
}

void DIV::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    int32_t rs1Value = static_cast<int32_t>(cpu.getRegister(rs1));
    int32_t rs2Value = static_cast<int32_t>(cpu.getRegister(rs2));
    uint32_t result;

    if (rs2Value == 0) {
        result = 0xFFFFFFFF;
    } else if (rs1Value == std::numeric_limits<int32_t>::min() && rs2Value == -1) {
        result = static_cast<uint32_t>(rs1Value);
    } else {
        result = static_cast<uint32_t>(rs1Value / rs2Value);
    }

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed DIV: x{} = x{} ({}) / x{} ({})",
            rd, rs1, rs1Value, rs2, rs2Value
        );
        instructionOutput->setRegisters({rs1, rs2, rd});
    }
}

void DIVU::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);
    uint32_t result;

    if (rs2Value == 0) {
        result = 0xFFFFFFFF;
    } else {
        result = rs1Value / rs2Value;
    }

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed DIVU: x{} = x{} ({}) / x{} ({})",
            rd, rs1, rs1Value, rs2, rs2Value
        );
        instructionOutput->setRegisters({rs1, rs2, rd});
    }
}

void REM::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    int32_t rs1Value = static_cast<int32_t>(cpu.getRegister(rs1));
    int32_t rs2Value = static_cast<int32_t>(cpu.getRegister(rs2));
    uint32_t result;

    if (rs2Value == 0) {
        result = static_cast<uint32_t>(rs1Value);
    } else if (rs1Value == std::numeric_limits<int32_t>::min() && rs2Value == -1) {
        result = 0;
    } else {
        result = static_cast<uint32_t>(rs1Value % rs2Value);
    }

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed REM: x{} = x{} ({}) % x{} ({})",
            rd, rs1, rs1Value, rs2, rs2Value
        );
        instructionOutput->setRegisters({rs1, rs2, rd});
    }
}

void REMU::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t rs1Value = cpu.getRegister(rs1);
    uint32_t rs2Value = cpu.getRegister(rs2);
    uint32_t result;

    if (rs2Value == 0) {
        result = rs1Value;
    } else {
        result = rs1Value % rs2Value;
    }

    cpu.setRegister(rd, result);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed REMU: x{} = x{} ({}) % x{} ({})",
            rd, rs1, rs1Value, rs2, rs2Value
        );
        instructionOutput->setRegisters({rs1, rs2, rd});
    }
}

// RV32A Instructions

std::unique_ptr<Instruction> AtomicInstructionFactory::create(uint32_t encodedInstruction)
{
    static const std::unordered_map<InstructionDescriptor, std::function<std::unique_ptr<Instruction>(uint32_t)>, InstructionDescriptor::InstructionDescriptorHash> instructionMap = {
        { LR::getInstructionDescriptor(), [](uint32_t ins) { return getBits(ins, 20, 24) == 0 ? std::make_unique<LR>(ins) : nullptr; }},
        { SC::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<SC>(ins); }},

        { AMO::SWAP::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<AMO::SWAP>(ins); }},
        {  AMO::ADD::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<AMO::ADD> (ins); }},
        {  AMO::XOR::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<AMO::XOR> (ins); }},
        {  AMO::AND::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<AMO::AND> (ins); }},
        {   AMO::OR::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<AMO::OR>  (ins); }},
        {  AMO::MIN::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<AMO::MIN> (ins); }},
        {  AMO::MAX::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<AMO::MAX> (ins); }},
        { AMO::MINU::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<AMO::MINU>(ins); }},
        { AMO::MAXU::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<AMO::MAXU>(ins); }}
    };

    uint8_t funct3 = getBits(encodedInstruction, 12, 14);
    uint8_t funct7 = getBits(encodedInstruction, 25, 31);

    uint8_t amo_op = funct7 >> 2;

    InstructionDescriptor descriptor{funct3, amo_op};

    auto it = instructionMap.find(descriptor);
    if (it != instructionMap.end())
        return it->second(encodedInstruction);

    return nullptr;
}

void LR::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    Memory& mem = Memory::getInstance();
    uint32_t virtual_address = cpu.getRegister(rs1);
    uint32_t value = mem.read32(virtual_address);

    uint32_t physical_address = mem.translateAddress(virtual_address, AccessType::Load);

    cpu.makeReservation(physical_address);

    if (rd != 0) {
        cpu.setRegister(rd, value);
    }

    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed LR.W: x{} <- Mem[0x{:X}], Reservation Set", rd, virtual_address
        );
        instructionOutput->modifiedRamAddresses.push_back({virtual_address, value});
        instructionOutput->setRegisters({rs1, rd});
    }
}

void SC::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput)
{
    uint32_t virtual_address = cpu.getRegister(rs1);
    Memory& mem = Memory::getInstance();

    uint32_t physical_address = mem.translateAddress(virtual_address, AccessType::Load);

    if (cpu.checkAndClearReservation(physical_address)) {
        uint32_t value_to_write = cpu.getRegister(rs2);
        mem.write32(virtual_address, value_to_write);
        if (rd != 0) cpu.setRegister(rd, 0);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed SC.W: SUCCESS, Mem[0x{:X}] <- {} (x{}), x{} <- 0",
                virtual_address, value_to_write, rs2, rd
            );
            instructionOutput->modifiedRamAddresses.push_back({virtual_address, value_to_write});
            instructionOutput->setRegisters({rs1, rs2, rd});
        }
    } else {
        if (rd != 0) cpu.setRegister(rd, 1);

        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format(
                "Performed SC.W: FAILED (Reservation lost or mismatch), x{} <- 1", rd
            );
            instructionOutput->setRegisters({rs1, rd});
        }
    }

    cpu.setPc(cpu.getPc() + 4);
}

void AMO::Instruction::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput) {
    Memory& mem = Memory::getInstance();

    uint32_t memory_address = cpu.getRegister(rs1);
    uint32_t value_from_rs2 = cpu.getRegister(rs2);

    uint32_t old_value = mem.read32(memory_address);
    uint32_t new_value = performAmoOperation(old_value, value_from_rs2);

    cpu.notifyStore(memory_address, 4);

    mem.write32(memory_address, new_value);

    if (rd != 0) {
        cpu.setRegister(rd, old_value);
    }

    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format(
            "Performed {}.W: Mem[0x{:X}] <- {} (old: {}, rs2: {}), old_val -> x{}",
            getInstructionName(), memory_address, new_value, old_value, value_from_rs2, rd
        );
        instructionOutput->modifiedRamAddresses.push_back({memory_address, new_value});
        instructionOutput->setRegisters({rs1, rs2, rd});
    }
}

} // namespace RType
