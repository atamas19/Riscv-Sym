#include <core/instruction/SystemInstruction.h>

#include <core/RiscvCpu.h>
#include <core/CsrUnit.h>

namespace System {

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

void CSRRW::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) {
    CsrUnit& csr = cpu.getCsr();
    
    // CSRRW mereu suprascrie CSR-ul, deci isWrite = true
    if (!csr.canAccess(csr_addr, cpu.getPrivilegeMode(), true)) {
        // TODO: cpu.takeTrap(Exception::IllegalInstruction);
        return;
    }

    uint32_t rs1_value = cpu.getRegister(rs1);

    // Citim valoarea veche doar dacă rd != 0 (altfel generăm efecte secundare degeaba)
    if (rd != 0) {
        uint32_t old_value = csr.read(csr_addr);
        cpu.setRegister(rd, old_value);
    }

    csr.write(csr_addr, rs1_value);

    if (csr_addr == 0x180) {
        Memory::getInstance().setSATP(csr.read(0x180));
    }

    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "CSRRW: CSR[0x" + std::to_string(csr_addr) + "]";
}

void CSRRS::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) {
    CsrUnit& csr = cpu.getCsr();
    
    // Dacă rs1 == 0, instrucțiunea nu modifică CSR-ul (este doar un read)
    bool isWrite = (rs1 != 0);
    
    if (!csr.canAccess(csr_addr, cpu.getPrivilegeMode(), isWrite)) {
        // TODO: cpu.takeTrap(Exception::IllegalInstruction);
        return;
    }

    uint32_t old_val = csr.read(csr_addr);
    uint32_t rs1_val = cpu.getRegister(rs1);

    if (isWrite) {
        csr.write(csr_addr, old_val | rs1_val);

        if (csr_addr == 0x180) {
            Memory::getInstance().setSATP(csr.read(0x180));
        }
    }
    
    cpu.setRegister(rd, old_val);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "CSRRS: Read/Set CSR[0x" + std::to_string(csr_addr) + "]";
}

void CSRRC::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) {
    CsrUnit& csr = cpu.getCsr();
    
    bool isWrite = (rs1 != 0);
    
    if (!csr.canAccess(csr_addr, cpu.getPrivilegeMode(), isWrite)) {
        // TODO: cpu.takeTrap(Exception::IllegalInstruction);
        return;
    }

    uint32_t old_val = csr.read(csr_addr);
    uint32_t rs1_val = cpu.getRegister(rs1);

    if (isWrite) {
        csr.write(csr_addr, old_val & ~rs1_val);

        if (csr_addr == 0x180) {
            Memory::getInstance().setSATP(csr.read(0x180));
        }
    }

    cpu.setRegister(rd, old_val);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "CSRRC: Read/Clear CSR[0x" + std::to_string(csr_addr) + "]";
}

void CSRRWI::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) {
    CsrUnit& csr = cpu.getCsr();
    
    if (!csr.canAccess(csr_addr, cpu.getPrivilegeMode(), true)) {
        // TODO: cpu.takeTrap(Exception::IllegalInstruction);
        return;
    }

    uint32_t uimm = rs1;

    if (rd != 0) {
        uint32_t old_value = csr.read(csr_addr);
        cpu.setRegister(rd, old_value);
    }
    csr.write(csr_addr, uimm);

    if (csr_addr == 0x180) {
        Memory::getInstance().setSATP(csr.read(0x180));
    }

    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "CSRRWI: CSR[0x" + std::to_string(csr_addr) + "]";
}

void CSRRSI::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) {
    CsrUnit& csr = cpu.getCsr();
    uint32_t uimm = rs1;
    
    bool isWrite = (uimm != 0);
    
    if (!csr.canAccess(csr_addr, cpu.getPrivilegeMode(), isWrite)) {
        // TODO: cpu.takeTrap(Exception::IllegalInstruction);
        return;
    }

    uint32_t old_val = csr.read(csr_addr);

    if (isWrite) {
        csr.write(csr_addr, old_val | uimm);

        if (csr_addr == 0x180) {
            Memory::getInstance().setSATP(csr.read(0x180));
        }
    }

    cpu.setRegister(rd, old_val);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "CSRRSI: Read/Set CSR[0x" + std::to_string(csr_addr) + "]";
}

void CSRRCI::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) {
    CsrUnit& csr = cpu.getCsr();
    uint32_t uimm = rs1;
    
    bool isWrite = (uimm != 0);
    
    if (!csr.canAccess(csr_addr, cpu.getPrivilegeMode(), isWrite)) {
        // TODO: cpu.takeTrap(Exception::IllegalInstruction);
        return;
    }

    uint32_t old_val = csr.read(csr_addr);

    if (isWrite) {
        csr.write(csr_addr, old_val & ~uimm);

        if (csr_addr == 0x180) {
            Memory::getInstance().setSATP(csr.read(0x180));
        }
    }

    cpu.setRegister(rd, old_val);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "CSRRCI: Read/Clear CSR[0x" + std::to_string(csr_addr) + "]";
}

void ECALL::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) {
    instructionOutput.consoleLog = "ECALL triggered";
    
    // Cauza depinde de modul din care se apelează
    PrivilegeMode currentMode = cpu.getPrivilegeMode();
    ExceptionCause cause;
    
    if (currentMode == PrivilegeMode::User)        cause = ExceptionCause::EnvironmentCallFromUMode;
    else if (currentMode == PrivilegeMode::Supervisor) cause = ExceptionCause::EnvironmentCallFromSMode;
    else                                           cause = ExceptionCause::EnvironmentCallFromMMode;

    cpu.takeTrap(cause);
}

void EBREAK::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) {
    instructionOutput.consoleLog = "EBREAK triggered";
    cpu.takeTrap(ExceptionCause::Breakpoint);
}

void MRET::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) {
    if (cpu.getPrivilegeMode() < PrivilegeMode::Machine) {
        cpu.takeTrap(ExceptionCause::IllegalInstruction);
        return;
    }
    instructionOutput.consoleLog = "MRET Executed";
    cpu.returnFromTrap(PrivilegeMode::Machine);
}

void SRET::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) {
    if (cpu.getPrivilegeMode() < PrivilegeMode::Supervisor) {
        cpu.takeTrap(ExceptionCause::IllegalInstruction);
        return;
    }
    instructionOutput.consoleLog = "SRET Executed";
    cpu.returnFromTrap(PrivilegeMode::Supervisor);
}

void SFENCE_VMA::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) {
    // În viitor, dacă implementezi un cache TLB, aici vei apela mmu.flushTLB()
    cpu.setPc(cpu.getPc() + 4);
    instructionOutput.consoleLog = "SFENCE.VMA: TLB Flushed";
};

} // namespace System
