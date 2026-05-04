#include <core/instruction/SystemInstruction.h>
#include <core/instruction/ITypeInstruction.h>

#include <core/RiscvCpu.h>
#include <core/CsrUnit.h>

#include <spdlog/fmt/fmt.h>

namespace System {

namespace InstructionNew
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

        cpu.setPc(cpu.getPc() + 4);

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
        cpu.setPc(cpu.getPc() + 4);

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
        cpu.setPc(cpu.getPc() + 4);

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

        cpu.setPc(cpu.getPc() + 4);

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
        cpu.setPc(cpu.getPc() + 4);

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
        cpu.setPc(cpu.getPc() + 4);

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
        if (instructionOutput) {
            instructionOutput->consoleLog = "EBREAK triggered";
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
        cpu.setPc(cpu.getPc() + 4);

        if (instructionOutput) {
            instructionOutput->consoleLog = "SFENCE.VMA: TLB Flushed";
        }
        return true;
    }
}

void Instruction::decode() {
    IType::Instruction::decode();

    csr_addr = static_cast<uint16_t>(imm & 0xFFF);
}

std::unique_ptr<Instruction> InstructionFactory::create(uint32_t encodedInstruction) {
    static const std::unordered_map<uint8_t, std::function<std::unique_ptr<Instruction>(uint32_t)>> zicsrInstructionMap = {
        { CSRRW ::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<CSRRW> (ins); }},
        { CSRRS ::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<CSRRS> (ins); }},
        { CSRRC ::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<CSRRC> (ins); }},
        { CSRRWI::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<CSRRWI>(ins); }},
        { CSRRSI::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<CSRRSI>(ins); }},
        { CSRRCI::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<CSRRCI>(ins); }}
    };

    uint8_t funct3 = getBits(encodedInstruction, 12, 14);

    if (funct3 == 0) {
        uint32_t funct12 = getBits(encodedInstruction, 20, 31);
        uint8_t funct7 = getBits(encodedInstruction, 25, 31);
        if (funct7 == SFENCE_VMA::getInstructionDescriptor()) {
            return std::make_unique<SFENCE_VMA>(encodedInstruction);
        }

        switch (funct12) {
            case ECALL::getInstructionDescriptor():  return std::make_unique<ECALL> (encodedInstruction);
            case EBREAK::getInstructionDescriptor(): return std::make_unique<EBREAK>(encodedInstruction);
            case MRET::getInstructionDescriptor():   return std::make_unique<MRET>(encodedInstruction);
            case SRET::getInstructionDescriptor():   return std::make_unique<SRET>(encodedInstruction);
            default:    return nullptr;
        }
    }

    auto it = zicsrInstructionMap.find(funct3);
    if (it != zicsrInstructionMap.end())
        return it->second(encodedInstruction);

    return nullptr;
}

void CSRRW::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput) {
    CsrUnit& csr = cpu.getCsr();

    if (!csr.canAccess(csr_addr, cpu.getPrivilegeMode(), true)) {
        cpu.takeTrap(ExceptionCause::IllegalInstruction, instruction);
        return;
    }

    uint32_t rs1_value = cpu.getRegister(rs1);

    if (rd != 0) {
        uint32_t old_value = csr.read(csr_addr);
        cpu.setRegister(rd, old_value);
    }

    csr.write(csr_addr, rs1_value);

    if (csr_addr == CsrAddress::SATP) {
        Memory::getInstance().setSATP(csr.read(CsrAddress::SATP));
    }

    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format("CSRRW: CSR[0x{:03X}]", csr_addr);
    }
}

void CSRRS::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput) {
    CsrUnit& csr = cpu.getCsr();

    bool isWrite = (rs1 != 0);

    if (!csr.canAccess(csr_addr, cpu.getPrivilegeMode(), isWrite)) {
        cpu.takeTrap(ExceptionCause::IllegalInstruction, instruction);
        return;
    }

    uint32_t old_val = csr.read(csr_addr);
    uint32_t rs1_val = cpu.getRegister(rs1);

    if (isWrite) {
        csr.write(csr_addr, old_val | rs1_val);

        if (csr_addr == CsrAddress::SATP) {
            Memory::getInstance().setSATP(csr.read(CsrAddress::SATP));
        }
    }

    cpu.setRegister(rd, old_val);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format("CSRRS: Read/Set CSR[0x{:03X}]", csr_addr);
    }
}

void CSRRC::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput) {
    CsrUnit& csr = cpu.getCsr();

    bool isWrite = (rs1 != 0);

    if (!csr.canAccess(csr_addr, cpu.getPrivilegeMode(), isWrite)) {
        cpu.takeTrap(ExceptionCause::IllegalInstruction, instruction);
        return;
    }

    uint32_t old_val = csr.read(csr_addr);
    uint32_t rs1_val = cpu.getRegister(rs1);

    if (isWrite) {
        csr.write(csr_addr, old_val & ~rs1_val);

        if (csr_addr == CsrAddress::SATP) {
            Memory::getInstance().setSATP(csr.read(CsrAddress::SATP));
        }
    }

    cpu.setRegister(rd, old_val);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format("CSRRC: Read/Clear CSR[0x{:03X}]", csr_addr);
    }
}

void CSRRWI::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput) {
    CsrUnit& csr = cpu.getCsr();

    if (!csr.canAccess(csr_addr, cpu.getPrivilegeMode(), true)) {
        cpu.takeTrap(ExceptionCause::IllegalInstruction, instruction);
        return;
    }

    uint32_t uimm = rs1;

    if (rd != 0) {
        uint32_t old_value = csr.read(csr_addr);
        cpu.setRegister(rd, old_value);
    }
    csr.write(csr_addr, uimm);

    if (csr_addr == CsrAddress::SATP) {
        Memory::getInstance().setSATP(csr.read(CsrAddress::SATP));
    }

    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format("CSRRWI: CSR[0x{:03X}]", csr_addr);
    }
}

void CSRRSI::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput) {
    CsrUnit& csr = cpu.getCsr();
    uint32_t uimm = rs1;

    bool isWrite = (uimm != 0);

    if (!csr.canAccess(csr_addr, cpu.getPrivilegeMode(), isWrite)) {
        cpu.takeTrap(ExceptionCause::IllegalInstruction, instruction);
        return;
    }

    uint32_t old_val = csr.read(csr_addr);

    if (isWrite) {
        csr.write(csr_addr, old_val | uimm);

        if (csr_addr == CsrAddress::SATP) {
            Memory::getInstance().setSATP(csr.read(CsrAddress::SATP));
        }
    }

    cpu.setRegister(rd, old_val);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format("CSRRSI: Read/Set CSR[0x{:03X}]", csr_addr);
    }
}

void CSRRCI::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput) {
    CsrUnit& csr = cpu.getCsr();
    uint32_t uimm = rs1;

    bool isWrite = (uimm != 0);

    if (!csr.canAccess(csr_addr, cpu.getPrivilegeMode(), isWrite)) {
        cpu.takeTrap(ExceptionCause::IllegalInstruction, instruction);
        return;
    }

    uint32_t old_val = csr.read(csr_addr);

    if (isWrite) {
        csr.write(csr_addr, old_val & ~uimm);

        if (csr_addr == CsrAddress::SATP) {
            Memory::getInstance().setSATP(csr.read(CsrAddress::SATP));
        }
    }

    cpu.setRegister(rd, old_val);
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = fmt::format("CSRRCI: Read/Clear CSR[0x{:03X}]", csr_addr);
    }
}

void ECALL::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput) {
    if (instructionOutput) {
        instructionOutput->consoleLog = "ECALL triggered";
    }

    PrivilegeMode currentMode = cpu.getPrivilegeMode();
    ExceptionCause cause;

    if (currentMode == PrivilegeMode::User)
        cause = ExceptionCause::EnvironmentCallFromUMode;
    else if (currentMode == PrivilegeMode::Supervisor)
        cause = ExceptionCause::EnvironmentCallFromSMode;
    else
        cause = ExceptionCause::EnvironmentCallFromMMode;

    cpu.takeTrap(cause);
}

void EBREAK::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput) {
    if (instructionOutput) {
        instructionOutput->consoleLog = "EBREAK triggered";
    }
    cpu.takeTrap(ExceptionCause::Breakpoint);
}

void MRET::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput) {
    if (cpu.getPrivilegeMode() < PrivilegeMode::Machine) {
        cpu.takeTrap(ExceptionCause::IllegalInstruction, instruction);
        return;
    }
    if (instructionOutput) {
        instructionOutput->consoleLog = "MRET Executed";
    }
    cpu.returnFromTrap(PrivilegeMode::Machine);
}

void SRET::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput) {
    PrivilegeMode currentMode = cpu.getPrivilegeMode();
    if (currentMode == PrivilegeMode::User) {
        cpu.takeTrap(ExceptionCause::IllegalInstruction, instruction);
        return;
    }

    uint32_t mstatus = cpu.getCsr().read(CsrAddress::MSTATUS);
    bool tsr = (mstatus & (1 << 22)) != 0;

    if (cpu.getPrivilegeMode() == PrivilegeMode::Supervisor && tsr) {
        cpu.takeTrap(ExceptionCause::IllegalInstruction, instruction);
        return;
    }
    if (instructionOutput) {
        instructionOutput->consoleLog = "SRET Executed";
    }
    cpu.returnFromTrap(PrivilegeMode::Supervisor);
}

void SFENCE_VMA::execute(RiscvCpu& cpu, InstructionOutput* instructionOutput) {
    // SFENCE.VMA is not legal in U-mode (and may be further restricted by TVM).
    if (cpu.getPrivilegeMode() < PrivilegeMode::Supervisor) {
        cpu.takeTrap(ExceptionCause::IllegalInstruction, instruction);
        return;
    }
    // If I'll implement a cache TLB, here I'll call mmu.flushTLB()
    cpu.setPc(cpu.getPc() + 4);

    if (instructionOutput) {
        instructionOutput->consoleLog = "SFENCE.VMA: TLB Flushed";
    }
}

} // namespace System
