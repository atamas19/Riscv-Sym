#include <core/instruction/SystemInstruction.h>
#include <core/instruction/ITypeInstruction.h>

#include <core/RiscvCpu.h>
#include <core/CsrUnit.h>

#include <cstdio>
#include <string>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>

namespace System {

namespace {
    bool isSemihostingTrap(uint32_t pc) {
        if (pc < 4) {
            return false;
        }

        Memory& mem = Memory::getInstance();
        const uint32_t previousInsn = mem.read32(pc - 4, true);
        const uint32_t nextInsn = mem.read32(pc + 4, true);

        return previousInsn == 0x01f01013u && nextInsn == 0x40705013u;
    }

    std::string readCStringFromMemory(Memory& mem, uint32_t address) {
        std::string result;
        if (address == 0) {
            return result;
        }

        for (uint32_t offset = 0; ; ++offset) {
            const uint8_t byte = mem.read8(address + offset);
            if (byte == 0) {
                break;
            }
            result.push_back(static_cast<char>(byte));
        }
        return result;
    }

    bool handleSemihostingTrap(RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t pc = cpu.getPc();
        const uint32_t sysnum = cpu.getRegister(10);
        const uint32_t arg = cpu.getRegister(11);
        uint32_t ret = 0;
        Memory& mem = Memory::getInstance();

        switch (sysnum) {
            case 0x01: { /* SYSOPEN */
                if (arg == 0) {
                    ret = static_cast<uint32_t>(-1);
                    break;
                }

                const uint32_t fnameAddr = mem.read32(arg, false);
                const uint32_t mode = mem.read32(arg + 4, false);
                const std::string path = readCStringFromMemory(mem, fnameAddr);

                if (path == ":tt") {
                    ret = (mode == 0x4u) ? 1u : 0u;
                } else {
                    ret = static_cast<uint32_t>(-1);
                }
                break;
            }
            case 0x03: { /* SYSWRITEC */
                if (arg != 0) {
                    const char ch = static_cast<char>(mem.read8(arg));
                    std::fputc(ch, stdout);
                    std::fflush(stdout);
                }
                ret = 1u;
                break;
            }
            case 0x05: { /* SYSWRITE */
                if (arg == 0) {
                    ret = static_cast<uint32_t>(-1);
                    break;
                }

                const uint32_t fd = mem.read32(arg, false);
                const uint32_t dataAddr = mem.read32(arg + 4, false);
                const uint32_t len = mem.read32(arg + 8, false);
                (void)fd;
                if (dataAddr != 0 && len != 0) {
                    std::string payload;
                    payload.reserve(len);
                    for (uint32_t i = 0; i < len; ++i) {
                        payload.push_back(static_cast<char>(mem.read8(dataAddr + i)));
                    }
                    std::fwrite(payload.data(), 1, payload.size(), stdout);
                    std::fflush(stdout);
                }
                ret = 0u;
                break;
            }
            case 0x06: { /* SYSREAD */
                if (arg == 0) {
                    ret = static_cast<uint32_t>(-1);
                    break;
                }

                const uint32_t fd = mem.read32(arg, false);
                const uint32_t dataAddr = mem.read32(arg + 4, false);
                const uint32_t len = mem.read32(arg + 8, false);
                (void)fd;
                if (dataAddr != 0 && len != 0) {
                    int ch = std::fgetc(stdin);
                    if (ch != EOF) {
                        mem.write8(dataAddr, static_cast<uint8_t>(ch));
                        ret = 0u;
                    } else {
                        ret = static_cast<uint32_t>(-1);
                    }
                } else {
                    ret = 0u;
                }
                break;
            }
            case 0x07: { /* SYSREADC */
                const int ch = std::fgetc(stdin);
                ret = (ch == EOF) ? static_cast<uint32_t>(-1) : static_cast<uint32_t>(ch);
                break;
            }
            case 0x13: { /* SYSERRNO */
                ret = 0u;
                break;
            }
            default:
                ret = 0u;
                break;
        }

        cpu.setRegister(10, ret);
        cpu.setPc(pc + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = "Semihosting trap handled";
        }
        return true;
    }
}

namespace Instruction
{
    static InstructionArguments getInstructionArguments(uint32_t encodedInstruction) {
        const uint8_t rd  = getBits(encodedInstruction,  7, 11);
        const uint8_t rs1 = getBits(encodedInstruction, 15, 19);
        const int32_t imm = IType::getImm(encodedInstruction);
        const uint16_t csr_addr = static_cast<uint16_t>(imm & 0xFFF);

        return {imm, csr_addr, rs1, rd};
    }

    bool execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const InstructionArguments instructionArguments = getInstructionArguments(encodedInstruction);

        const uint8_t funct3 = getBits(encodedInstruction, 12, 14);
        if (funct3 == 0) {
            const uint32_t funct12 = getBits(encodedInstruction, 20, 31);
            const uint8_t funct7 = getBits(encodedInstruction, 25, 31);
            if (funct7 == SFENCE_VMA::getInstructionDescription()) {
                return SFENCE_VMA::execute(encodedInstruction, cpu, instructionOutput);
            }

            switch (funct12) {
                case ECALL::getInstructionDescription():  return ECALL::execute(cpu, instructionOutput);
                case EBREAK::getInstructionDescription(): return EBREAK::execute(cpu, instructionOutput);
                case MRET::getInstructionDescription():   return MRET::execute(encodedInstruction, cpu, instructionOutput);
                case SRET::getInstructionDescription():   return SRET::execute(encodedInstruction, cpu, instructionOutput);
                case WFI::getInstructionDescription():    return WFI::execute(cpu, instructionOutput);
            }
        }

        switch (funct3) {
            case CSRRW::getInstructionDescription() :  return CSRRW::execute(instructionArguments, encodedInstruction, cpu, instructionOutput);
            case CSRRS::getInstructionDescription() :  return CSRRS::execute(instructionArguments, encodedInstruction, cpu, instructionOutput);
            case CSRRC::getInstructionDescription() :  return CSRRC::execute(instructionArguments, encodedInstruction, cpu, instructionOutput);
            case CSRRWI::getInstructionDescription() : return CSRRWI::execute(instructionArguments, encodedInstruction, cpu, instructionOutput);
            case CSRRSI::getInstructionDescription() : return CSRRSI::execute(instructionArguments, encodedInstruction, cpu, instructionOutput);
            case CSRRCI::getInstructionDescription() : return CSRRCI::execute(instructionArguments, encodedInstruction, cpu, instructionOutput);
        }

        spdlog::warn("System instruction not recognized: funct3=0x{:x}, funct12=0x{:03x}, raw_insn=0x{:08x}",
                     funct3, getBits(encodedInstruction, 20, 31), encodedInstruction);
        return false;
    }

    bool CSRRW::execute(InstructionArguments instructionArguments, uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        CsrUnit& csr = cpu.getCsr();

        if (!csr.canAccess(instructionArguments.csr_addr, cpu.getPrivilegeMode(), true)) {
            cpu.takeTrap(ExceptionCause::IllegalInstruction, encodedInstruction);
            return true;
        }

        const uint32_t rs1_value = cpu.getRegister(instructionArguments.rs1);

        if (instructionArguments.rd != 0) {
            const uint32_t old_value = csr.read(instructionArguments.csr_addr);
            cpu.setRegister(instructionArguments.rd, old_value);
        }

        csr.write(instructionArguments.csr_addr, rs1_value);

        if (instructionArguments.csr_addr == CsrAddress::SATP) {
            Memory::getInstance().setSATP(csr.read(CsrAddress::SATP));
        }



        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format("CSRRW: CSR[0x{:03X}]", instructionArguments.csr_addr);
        }
        return true;
    }

    bool CSRRS::execute(InstructionArguments instructionArguments, uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        CsrUnit& csr = cpu.getCsr();

        const bool isWrite = (instructionArguments.rs1 != 0);

        if (!csr.canAccess(instructionArguments.csr_addr, cpu.getPrivilegeMode(), isWrite)) {
            cpu.takeTrap(ExceptionCause::IllegalInstruction, encodedInstruction);
            return true;
        }

        const uint32_t old_val = csr.read(instructionArguments.csr_addr);
        const uint32_t rs1_val = cpu.getRegister(instructionArguments.rs1);

        if (isWrite) {
            csr.write(instructionArguments.csr_addr, old_val | rs1_val);

            if (instructionArguments.csr_addr == CsrAddress::SATP) {
                Memory::getInstance().setSATP(csr.read(CsrAddress::SATP));
            }
        }

        cpu.setRegister(instructionArguments.rd, old_val);


        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format("CSRRS: Read/Set CSR[0x{:03X}]", instructionArguments.csr_addr);
        }
        return true;
    }

    bool CSRRC::execute(InstructionArguments instructionArguments, uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        CsrUnit& csr = cpu.getCsr();

        const bool isWrite = (instructionArguments.rs1 != 0);

        if (!csr.canAccess(instructionArguments.csr_addr, cpu.getPrivilegeMode(), isWrite)) {
            cpu.takeTrap(ExceptionCause::IllegalInstruction, encodedInstruction);
            return true;
        }

        const uint32_t old_val = csr.read(instructionArguments.csr_addr);
        const uint32_t rs1_val = cpu.getRegister(instructionArguments.rs1);

        if (isWrite) {
            csr.write(instructionArguments.csr_addr, old_val & ~rs1_val);

            if (instructionArguments.csr_addr == CsrAddress::SATP) {
                Memory::getInstance().setSATP(csr.read(CsrAddress::SATP));
            }
        }

        cpu.setRegister(instructionArguments.rd, old_val);


        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format("CSRRC: Read/Clear CSR[0x{:03X}]", instructionArguments.csr_addr);
        }
        return true;
    }

    bool CSRRWI::execute(InstructionArguments instructionArguments, uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        CsrUnit& csr = cpu.getCsr();

        if (!csr.canAccess(instructionArguments.csr_addr, cpu.getPrivilegeMode(), true)) {
            cpu.takeTrap(ExceptionCause::IllegalInstruction, encodedInstruction);
            return true;
        }

        const uint32_t uimm = instructionArguments.rs1;

        if (instructionArguments.rd != 0) {
            const uint32_t old_value = csr.read(instructionArguments.csr_addr);
            cpu.setRegister(instructionArguments.rd, old_value);
        }
        csr.write(instructionArguments.csr_addr, uimm);

        if (instructionArguments.csr_addr == CsrAddress::SATP) {
            Memory::getInstance().setSATP(csr.read(CsrAddress::SATP));
        }



        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format("CSRRWI: CSR[0x{:03X}]", instructionArguments.csr_addr);
        }
        return true;
    }

    bool CSRRSI::execute(InstructionArguments instructionArguments, uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        CsrUnit& csr = cpu.getCsr();
        const uint32_t uimm = instructionArguments.rs1;

        const bool isWrite = (uimm != 0);

        if (!csr.canAccess(instructionArguments.csr_addr, cpu.getPrivilegeMode(), isWrite)) {
            cpu.takeTrap(ExceptionCause::IllegalInstruction, encodedInstruction);
            return true;
        }

        const uint32_t old_val = csr.read(instructionArguments.csr_addr);

        if (isWrite) {
            csr.write(instructionArguments.csr_addr, old_val | uimm);

            if (instructionArguments.csr_addr == CsrAddress::SATP) {
                Memory::getInstance().setSATP(csr.read(CsrAddress::SATP));
            }
        }

        cpu.setRegister(instructionArguments.rd, old_val);


        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format("CSRRSI: Read/Set CSR[0x{:03X}]", instructionArguments.csr_addr);
        }
        return true;
    }

    bool CSRRCI::execute(InstructionArguments instructionArguments, uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        CsrUnit& csr = cpu.getCsr();
        const uint32_t uimm = instructionArguments.rs1;

        const bool isWrite = (uimm != 0);

        if (!csr.canAccess(instructionArguments.csr_addr, cpu.getPrivilegeMode(), isWrite)) {
            cpu.takeTrap(ExceptionCause::IllegalInstruction, encodedInstruction);
            return true;
        }

        const uint32_t old_val = csr.read(instructionArguments.csr_addr);

        if (isWrite) {
            csr.write(instructionArguments.csr_addr, old_val & ~uimm);

            if (instructionArguments.csr_addr == CsrAddress::SATP) {
                Memory::getInstance().setSATP(csr.read(CsrAddress::SATP));
            }
        }

        cpu.setRegister(instructionArguments.rd, old_val);


        if (instructionOutput) {
            instructionOutput->consoleLog = fmt::format("CSRRCI: Read/Clear CSR[0x{:03X}]", instructionArguments.csr_addr);
        }
        return true;
    }

    bool ECALL::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        if (instructionOutput) {
            instructionOutput->consoleLog = "ECALL triggered";
        }

        const PrivilegeMode currentMode = cpu.getPrivilegeMode();
        ExceptionCause cause;

        if (currentMode == PrivilegeMode::User) {
            cause = ExceptionCause::EnvironmentCallFromUMode;
        } else if (currentMode == PrivilegeMode::Supervisor) {
            cause = ExceptionCause::EnvironmentCallFromSMode;
        } else {
            cause = ExceptionCause::EnvironmentCallFromMMode;
        }

        cpu.takeTrap(cause);
        return true;
    }

    bool EBREAK::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const uint32_t pc = cpu.getPc();

        if (isSemihostingTrap(pc)) {
            return handleSemihostingTrap(cpu, instructionOutput);
        }

        if (instructionOutput) {
            instructionOutput->consoleLog = "EBREAK trap";
        }

        cpu.takeTrap(ExceptionCause::Breakpoint);
        return true;
    }

    bool MRET::execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        if (cpu.getPrivilegeMode() < PrivilegeMode::Machine) {
            cpu.takeTrap(ExceptionCause::IllegalInstruction, encodedInstruction);
            return true;
        }
        if (instructionOutput) {
            instructionOutput->consoleLog = "MRET Executed";
        }
        cpu.returnFromTrap(PrivilegeMode::Machine);
        return true;
    }

    bool SRET::execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        const PrivilegeMode currentMode = cpu.getPrivilegeMode();
        if (currentMode == PrivilegeMode::User) {
            cpu.takeTrap(ExceptionCause::IllegalInstruction, encodedInstruction);
            return true;
        }

        const uint32_t mstatus = cpu.getCsr().read(CsrAddress::MSTATUS);
        const bool tsr = (mstatus & (1 << 22)) != 0;

        if (cpu.getPrivilegeMode() == PrivilegeMode::Supervisor && tsr) {
            cpu.takeTrap(ExceptionCause::IllegalInstruction, encodedInstruction);
            return true;
        }
        if (instructionOutput) {
            instructionOutput->consoleLog = "SRET Executed";
        }
        cpu.returnFromTrap(PrivilegeMode::Supervisor);
        return true;
    }

    bool SFENCE_VMA::execute(uint32_t encodedInstruction, RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        // SFENCE.VMA is not legal in U-mode (and may be further restricted by TVM).
        if (cpu.getPrivilegeMode() < PrivilegeMode::Supervisor) {
            cpu.takeTrap(ExceptionCause::IllegalInstruction, encodedInstruction);
            return true;
        }
        // If I'll implement a cache TLB, here I'll call mmu.flushTLB()


        if (instructionOutput) {
            instructionOutput->consoleLog = "SFENCE.VMA: TLB Flushed";
        }
        return true;
    }

    bool WFI::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput) {
        // Wait For Interrupt: Pause execution until an interrupt is received.
        // For a simulator, we just skip the instruction and continue.
        // In a real implementation, this would put the CPU in a low-power sleep mode.


        if (instructionOutput) {
            instructionOutput->consoleLog = "WFI: Wait For Interrupt";
        }
        return true;
    }

} // namespace Instruction

} // namespace System
