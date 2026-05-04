#include <core/instruction/RTypeInstruction.h>
#include <core/RiscvCpu.h>

#include <unordered_map>
#include <limits>

#include <spdlog/fmt/fmt.h>

namespace RType
{

static InstructionArguments getInstructionArguments(uint32_t encodedInstruction) {
    const uint8_t rd  = getBits(encodedInstruction, 7, 11);
    const uint8_t rs1 = getBits(encodedInstruction, 15, 19);
    const uint8_t rs2 = getBits(encodedInstruction, 20, 24);

    return {rs1, rs2, rd};
}

namespace Instruction
{
    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const InstructionArguments instructionArguments = getInstructionArguments(encodedInstruction);

        const uint8_t funct3 = getBits(encodedInstruction, 12, 14);
        const uint8_t funct7 = getBits(encodedInstruction, 25, 31);
        const uint16_t instructionDescriptor = createRuntimeInstructionDescription(funct3, funct7);
        switch (instructionDescriptor)
        {
        case ADD::getInstructionDescription():
            return ADD::execute(instructionArguments, cpu, instructionOutput);
        case SUB::getInstructionDescription():
            return SUB::execute(instructionArguments, cpu, instructionOutput);
        case SLL::getInstructionDescription():
            return SLL::execute(instructionArguments, cpu, instructionOutput);
        case SLT::getInstructionDescription():
            return SLT::execute(instructionArguments, cpu, instructionOutput);
        case SLTU::getInstructionDescription():
            return SLTU::execute(instructionArguments, cpu, instructionOutput);
        case XOR::getInstructionDescription():
            return XOR::execute(instructionArguments, cpu, instructionOutput);
        case SRL::getInstructionDescription():
            return SRL::execute(instructionArguments, cpu, instructionOutput);
        case SRA::getInstructionDescription():
            return SRA::execute(instructionArguments, cpu, instructionOutput);
        case OR::getInstructionDescription():
            return OR::execute(instructionArguments, cpu, instructionOutput);
        case AND::getInstructionDescription():
            return AND::execute(instructionArguments, cpu, instructionOutput);
        case MUL::getInstructionDescription():
            return MUL::execute(instructionArguments, cpu, instructionOutput);
        case MULH::getInstructionDescription():
            return MULH::execute(instructionArguments, cpu, instructionOutput);
        case MULHSU::getInstructionDescription():
            return MULHSU::execute(instructionArguments, cpu, instructionOutput);
        case MULHU::getInstructionDescription():
            return MULHU::execute(instructionArguments, cpu, instructionOutput);
        case DIV::getInstructionDescription():
            return DIV::execute(instructionArguments, cpu, instructionOutput);
        case DIVU::getInstructionDescription():
            return DIVU::execute(instructionArguments, cpu, instructionOutput);
        case REM::getInstructionDescription():
            return REM::execute(instructionArguments, cpu, instructionOutput);
        case REMU::getInstructionDescription():
            return REMU::execute(instructionArguments, cpu, instructionOutput);
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

} // namespace Instruction

namespace AtomicInstruction
{
    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const InstructionArguments instructionArguments = getInstructionArguments(encodedInstruction);

        const uint8_t funct3 = getBits(encodedInstruction, 12, 14);
        const uint8_t funct7 = getBits(encodedInstruction, 25, 31);
        const uint8_t amo_op = funct7 >> 2;

        const uint16_t instructionDescriptor = createRuntimeInstructionDescription(funct3, amo_op);
        switch (instructionDescriptor) {
            case LR::getInstructionDescription(): return LR::execute(instructionArguments, cpu, instructionOutput);
            case SC::getInstructionDescription(): return SC::execute(instructionArguments, cpu, instructionOutput);

            case AMO::ADD:: getInstructionDescription(): return AMO::ADD:: execute(instructionArguments, cpu, instructionOutput);
            case AMO::SWAP::getInstructionDescription(): return AMO::SWAP::execute(instructionArguments, cpu, instructionOutput);
            case AMO::XOR:: getInstructionDescription(): return AMO::XOR:: execute(instructionArguments, cpu, instructionOutput);
            case AMO::OR::  getInstructionDescription(): return AMO::OR::  execute(instructionArguments, cpu, instructionOutput);
            case AMO::AND:: getInstructionDescription(): return AMO::AND:: execute(instructionArguments, cpu, instructionOutput);
            case AMO::MIN:: getInstructionDescription(): return AMO::MIN:: execute(instructionArguments, cpu, instructionOutput);
            case AMO::MAX:: getInstructionDescription(): return AMO::MAX:: execute(instructionArguments, cpu, instructionOutput);
            case AMO::MINU::getInstructionDescription(): return AMO::MINU::execute(instructionArguments, cpu, instructionOutput);
            case AMO::MAXU::getInstructionDescription(): return AMO::MAXU::execute(instructionArguments, cpu, instructionOutput);
        }
        return false;
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
    }

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
        }

        namespace ADD {
            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
                auto addFunction = [](uint32_t old_val, uint32_t rs2_val) { return old_val + rs2_val; };
                return AMO::execute({instructionArguments, "AMOADD", addFunction}, cpu, instructionOutput);
            }
        }

        namespace XOR {
            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
                auto xorFunction = [](uint32_t old_val, uint32_t rs2_val) { return old_val ^ rs2_val; };
                return AMO::execute({instructionArguments, "AMOXOR", xorFunction}, cpu, instructionOutput);
            }
        }

        namespace AND {
            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
                auto andFunction = [](uint32_t old_val, uint32_t rs2_val) { return old_val & rs2_val; };
                return AMO::execute({instructionArguments, "AMOAND", andFunction}, cpu, instructionOutput);
            }
        }

        namespace OR {
            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
                auto orFunction = [](uint32_t old_val, uint32_t rs2_val) { return old_val | rs2_val; };
                return AMO::execute({instructionArguments, "AMOOR", orFunction}, cpu, instructionOutput);
            }
        }

        namespace MIN {
            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
                auto minFunction = [](uint32_t old_val, uint32_t rs2_val) {
                    int32_t signed_old = static_cast<int32_t>(old_val);
                    int32_t signed_rs2 = static_cast<int32_t>(rs2_val);
                    return static_cast<uint32_t>(std::min(signed_old, signed_rs2));
                };
                return AMO::execute({instructionArguments, "AMOMIN", minFunction}, cpu, instructionOutput);
            }
        }

        namespace MAX {
            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
                auto maxFunction = [](uint32_t old_val, uint32_t rs2_val) {
                    int32_t signed_old = static_cast<int32_t>(old_val);
                    int32_t signed_rs2 = static_cast<int32_t>(rs2_val);
                    return static_cast<uint32_t>(std::max(signed_old, signed_rs2));
                };
                return AMO::execute({instructionArguments, "AMOMAX", maxFunction}, cpu, instructionOutput);
            }
        }

        namespace MINU {
            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
                auto minuFunction = [](uint32_t old_val, uint32_t rs2_val) {
                    return std::min(old_val, rs2_val);
                };
                return AMO::execute({instructionArguments, "AMOMINU", minuFunction}, cpu, instructionOutput);
            }
        }

        namespace MAXU {
            bool execute(RType::InstructionArguments instructionArguments, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
                auto maxuFunction = [](uint32_t old_val, uint32_t rs2_val) {
                    return std::max(old_val, rs2_val);
                };
                return AMO::execute({instructionArguments, "AMOMAXU", maxuFunction}, cpu, instructionOutput);
            }
        }
    } // namespace AMO
} // namespace AtomicInstruction

} // namespace RType
