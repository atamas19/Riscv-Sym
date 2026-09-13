#include <core/RiscvCpu.h>
#include <core/AssemblyCompiler.h>
#include <core/instruction/CTypeInstruction.h>

#include <spdlog/spdlog.h>

#include <fstream>

RiscvCpu& RiscvCpu::getInstance()
{
    static RiscvCpu instance;

    return instance;
}

uint32_t RiscvCpu::getPc() const {
    return _pc;
}

uint32_t RiscvCpu::getNextPc() const {
    return _nextPc;
}


uint32_t RiscvCpu::getRegister(uint8_t registerIndex) const {
    return _regs.at(registerIndex);
}

CsrUnit& RiscvCpu::getCsr() {
    return _csrUnit;
}

PrivilegeMode RiscvCpu::getPrivilegeMode() const {
    return _privilegeMode;
}

void RiscvCpu::setPc(uint32_t pcValue) {
    _pc = pcValue;
}

void RiscvCpu::setNextPc(uint32_t val) {
    _nextPc = val;
}

void RiscvCpu::setRegister(uint8_t registerIndex, uint32_t registerValue) {
    if (registerIndex == 0) {
        return ;
    }

    _regs.at(registerIndex) = registerValue;
}

void RiscvCpu::setPrivilegeMode(PrivilegeMode mode) {
    _privilegeMode = mode;
}

int RiscvCpu::executeAsmCommand(const std::string& command, InstructionOutput& instructionOutput) {
    const uint32_t binaryInstruction = getBinaryInstructionFromAsmCommand(command, instructionOutput);

    if (!binaryInstruction) {
        instructionOutput.exitCode = -1;
        return 1;
    }

    Instruction::execute(binaryInstruction, *this, &instructionOutput);
    instructionOutput.exitCode = 0;

    return 0;
}

uint32_t RiscvCpu::getBinaryInstructionFromAsmCommand(const std::string& asmCommand, InstructionOutput& instructionOutput) {
    const uint32_t binaryInstruction = AssemblyCompiler::compile(asmCommand, &instructionOutput);

    spdlog::debug("Binary Instruction: {:032b}", binaryInstruction);

    if (!binaryInstruction) {
        instructionOutput.consoleLog = "Error converting `" + asmCommand + "` to binary: " + instructionOutput.consoleLog;
    }

    return binaryInstruction;
}

bool RiscvCpu::executeFromBinFile(const std::string& filePath, uint32_t startAddr) {
    if (!loadBinFileToMemory(filePath, startAddr)) {
        spdlog::error("Couldn't load bin file to memory!");
        return false;
    }

    spdlog::info("Instructions have been loaded. Starting execution...");

    this->_pc = startAddr;
    uint32_t prevSatp = getCsr().read(CsrAddress::SATP);
    uint32_t prevStvec = getCsr().read(CsrAddress::STVEC);
    uint32_t prevSepc = getCsr().read(CsrAddress::SEPC);
    uint32_t prevSscratch = getCsr().read(CsrAddress::SSCRATCH);
    uint32_t prevMstatus = getCsr().read(CsrAddress::MSTATUS);
    uint32_t csrTransitionLogCount = 0;
    auto logCsrTransitions = [&](const char* phase, uint32_t pc) {
        if (csrTransitionLogCount >= 160) {
            return;
        }

        const uint32_t satp = getCsr().read(CsrAddress::SATP);
        const uint32_t stvec = getCsr().read(CsrAddress::STVEC);
        const uint32_t sepc = getCsr().read(CsrAddress::SEPC);
        const uint32_t sscratch = getCsr().read(CsrAddress::SSCRATCH);
        const uint32_t mstatus = getCsr().read(CsrAddress::MSTATUS);

        if (satp != prevSatp || stvec != prevStvec || sepc != prevSepc ||
            sscratch != prevSscratch || mstatus != prevMstatus) {
            spdlog::info(
                "[CSR TRACE] {} pc=0x{:08X} satp=0x{:08X} stvec=0x{:08X}(mode={}) sepc=0x{:08X} sscratch=0x{:08X} mstatus=0x{:08X}",
                phase, pc, satp, stvec, stvec & 0x3, sepc, sscratch, mstatus);
            ++csrTransitionLogCount;
        }

        prevSatp = satp;
        prevStvec = stvec;
        prevSepc = sepc;
        prevSscratch = sscratch;
        prevMstatus = mstatus;
    };

    for (int i{0}; true; ++i) {
        // Check for shutdown request (e.g., from Ctrl+C)
        if (_shutdown_requested) {
            spdlog::info("Execution stopped by user");
            break;
        }

        // Poll keyboard more frequently - every 1000 instructions instead of 20000
        if (i % 100000 == 0) {
            _mem.pollKeyboard();
        }

        if (i % 100000 == 0) {
            // spdlog::info("[PROGRESS] pc=0x{:08X} mode={} instrs={}", this->_pc, static_cast<int>(getPrivilegeMode()), i);
        }

        // if (_mem.getUartInputChar() != -1) {
        //     PrivilegeMode currentMode = getPrivilegeMode();

        //     if (currentMode != PrivilegeMode::Machine) {
        //         uint32_t sstatus = getCsr().read(0x100);
        //         uint32_t sieCsr = getCsr().read(0x104);

        //         bool sieGlobal = (sstatus & 0x2) != 0;
        //         bool seie = (sieCsr & (1u << 9)) != 0;

        //         if (seie && (currentMode == PrivilegeMode::User ||
        //                     (currentMode == PrivilegeMode::Supervisor && sieGlobal))) {
        //             uint32_t sipCsr = getCsr().read(0x144);
        //             sipCsr |= (1u << 9);
        //             getCsr().write(0x144, sipCsr);

        //             takeTrap(static_cast<ExceptionCause>((uint32_t)0x80000009), 0);
        //             continue;
        //         }
        //     }
        // }

        try {
            uint32_t rawInstruction = _mem.read32(this->_pc, true);
            uint32_t binaryInstruction;
            bool isCompressed = ((rawInstruction & 0x3) != 0x3);

            _nextPc = this->_pc + (isCompressed ? 2 : 4);

            if (isCompressed) {
                binaryInstruction = CType::decompress(rawInstruction & 0xFFFF);
            } else {
                binaryInstruction = rawInstruction;
            }

            if (binaryInstruction == 0 && this->_pc == 0) break;

            // 2. EXECUTĂM instrucțiunea (cele de salt vor suprascrie _nextPc)
            if (!Instruction::execute(binaryInstruction, *this)) {
                takeTrap(ExceptionCause::IllegalInstruction, rawInstruction);
                continue;
            }

            // 3. AVANSĂM PC-ul
            this->_pc = _nextPc;
            _mem.incrementTime(1);
            // ... (restul logicii cu MIP și MIE rămâne intactă)

            // --- ACTUALIZARE MTIME ---
            uint32_t mip_csr = getCsr().read(CsrAddress::MIP);

            // 1. Verificăm timer-ul Machine (CLINT mtimecmp)
            if (_mem.getMtime() >= _mem.getMtimecmp()) {
                mip_csr |= (1u << 7); // Setăm MTIP
            } else {
                mip_csr &= ~(1u << 7); // Curățăm MTIP
            }

            // 2. Verificăm timer-ul Supervisor (Extensia SSTC)
            // CSR 0x14D este stimecmp (LOW), CSR 0x15D este stimecmph (HIGH)
            uint64_t stimecmp = static_cast<uint64_t>(getCsr().read(0x14D)) |
                               (static_cast<uint64_t>(getCsr().read(0x15D)) << 32);

            if (_mem.getMtime() >= stimecmp) {
                mip_csr |= (1u << 5); // Setăm STIP (Supervisor Timer Interrupt)
            } else {
                mip_csr &= ~(1u << 5); // Curățăm STIP
            }

            getCsr().setMIP(mip_csr);

            // --- ROUTER DE ÎNTRERUPERI RISC-V ---
            uint32_t mie_csr = getCsr().read(CsrAddress::MIE);
            uint32_t mstatus = getCsr().read(CsrAddress::MSTATUS);
            PrivilegeMode currentMode = getPrivilegeMode();

            // Verificăm global dacă întreruperile sunt activate pentru M și S mode
            bool m_enabled = (currentMode < PrivilegeMode::Machine) ||
                             ((currentMode == PrivilegeMode::Machine) && (mstatus & (1 << 3)));
            bool s_enabled = (currentMode < PrivilegeMode::Supervisor) ||
                             ((currentMode == PrivilegeMode::Supervisor) && (mstatus & (1 << 1)));

            // `pending` conține toți biții care sunt și ceruți (MIP) și permiși (MIE)
            uint32_t pending = mip_csr & mie_csr;
            uint32_t interrupt_cause = 0xFFFFFFFF;

            // 1. Prioritate Maximă: Întreruperi Machine (M-Mode)
            if (m_enabled) {
                if (pending & (1 << 11)) interrupt_cause = 11;      // MEI (External)
                else if (pending & (1 << 3)) interrupt_cause = 3;   // MSI (Software)
                else if (pending & (1 << 7)) interrupt_cause = 7;   // MTI (Timer)
            }

            // 2. Prioritate Secundară: Întreruperi Supervisor (S-Mode)
            // Se verifică doar dacă nu a fost deja declanșată o întrerupere de M-Mode
            if (interrupt_cause == 0xFFFFFFFF && s_enabled) {
                if (pending & (1 << 9)) interrupt_cause = 9;        // SEI (External)
                else if (pending & (1 << 1)) interrupt_cause = 1;   // SSI (Software)
                else if (pending & (1 << 5)) interrupt_cause = 5;   // STI (Timer)
            }

            // Dacă am găsit o întrerupere validă, o declanșăm asincron
            if (interrupt_cause != 0xFFFFFFFF) {
                // Adăugăm 0x80000000 pentru a marca că este Interrupt, nu Exception
                takeTrap(static_cast<ExceptionCause>(0x80000000 | interrupt_cause), 0);
                logCsrTransitions("post-interrupt-trap", this->_pc);
                continue;
            }

            logCsrTransitions("post-insn", this->_pc);

        } catch (const PageFaultException& e) {
            ExceptionCause cause;
            if (e.accessType == AccessType::InstructionFetch) {
                cause = ExceptionCause::InstructionPageFault;
            } else if (e.accessType == AccessType::Load) {
                cause = ExceptionCause::LoadPageFault;
            } else {
                cause = ExceptionCause::StorePageFault;
            }

            static uint32_t pageFaultLogCount = 0;
            if (pageFaultLogCount < 32) {
                const uint32_t satp = getCsr().read(CsrAddress::SATP);
                spdlog::info("[PAGE FAULT] pc=0x{:08X} vaddr=0x{:08X} accessType={} satp=0x{:08X}",
                             this->_pc, e.faultingAddress, static_cast<int>(e.accessType), satp);
                ++pageFaultLogCount;
            }

            takeTrap(cause, e.faultingAddress);
            logCsrTransitions("post-page-fault-trap", this->_pc);
        }
    }

    return true;
}

void RiscvCpu::takeTrap(ExceptionCause cause, uint32_t trapValue) {
    cancelReservation();

    uint32_t rawCause = static_cast<uint32_t>(cause);
    uint32_t causeIndex = rawCause & 0x7FFFFFFF;

    bool isInterrupt = (rawCause & 0x80000000) != 0;
    bool delegateToS = false;

    if (_privilegeMode <= PrivilegeMode::Supervisor) {
        if (isInterrupt) {
            uint32_t mideleg = _csrUnit.read(CsrAddress::MIDELEG);
            delegateToS = (mideleg & (1 << causeIndex)) != 0;
        } else {
            uint32_t medeleg = _csrUnit.read(CsrAddress::MEDELEG);
            delegateToS = (medeleg & (1 << causeIndex)) != 0;
        }
    }

    if (delegateToS) {
        // spdlog::info("Trap 0x{:08X} delegated to S-mode (pc=0x{:08X})", rawCause, _pc);

        // If this is an interrupt and it is delegated to S-mode, reflect the
        // pending bit in the S-mode pending register so supervisor code can
        // observe it (SIP) — write to SIP will update MIP via delegation mask.
        if (isInterrupt) {
            _csrUnit.write(CsrAddress::SIP, (1u << causeIndex));
        }

        _csrUnit.write(CsrAddress::SEPC, _pc);
        _csrUnit.write(CsrAddress::SCAUSE, rawCause);
        _csrUnit.write(CsrAddress::STVAL, trapValue);

        static uint32_t trapDebugCount = 0;
        if (trapDebugCount < 32 &&
            (rawCause == static_cast<uint32_t>(ExceptionCause::InstructionPageFault) ||
             rawCause == static_cast<uint32_t>(ExceptionCause::LoadPageFault) ||
             rawCause == static_cast<uint32_t>(ExceptionCause::StorePageFault) ||
             rawCause == static_cast<uint32_t>(ExceptionCause::Breakpoint))) {
            uint32_t mstatus = _csrUnit.read(CsrAddress::MSTATUS);
            uint32_t mie = _csrUnit.read(CsrAddress::MIE);
            uint32_t mip = _csrUnit.read(CsrAddress::MIP);
            uint32_t satp = _csrUnit.read(CsrAddress::SATP);
            spdlog::info("[TRAP DEBUG] cause=0x{:08X} pc=0x{:08X} stvec=0x{:08X} satp=0x{:08X} mstatus=0x{:08X} mie=0x{:08X} mip=0x{:08X}",
                         rawCause, _pc, _csrUnit.read(CsrAddress::STVEC), satp, mstatus, mie, mip);
            ++trapDebugCount;
        }

        uint32_t sstatus = _csrUnit.read(CsrAddress::SSTATUS);
        uint32_t spp = (static_cast<uint32_t>(_privilegeMode) & 1);
        uint32_t sie_bit = (sstatus >> 1) & 1;

        sstatus = (sstatus & ~((1u << 1) | (1u << 5) | (1u << 8))) | (sie_bit << 5) | (spp << 8);
        _csrUnit.write(CsrAddress::SSTATUS, sstatus);

        _privilegeMode = PrivilegeMode::Supervisor;
        _pc = _csrUnit.read(CsrAddress::STVEC) & ~0x3;

        _nextPc = _pc;

    } else {
        spdlog::info("Trap 0x{:08X} handled in M-mode (pc=0x{:08X})", rawCause, _pc);

        _csrUnit.write(CsrAddress::MEPC, _pc);
        _csrUnit.write(CsrAddress::MCAUSE, rawCause);
        _csrUnit.write(CsrAddress::MTVAL, trapValue);

        uint32_t mstatus = _csrUnit.read(CsrAddress::MSTATUS);

        uint32_t mpp = (static_cast<uint32_t>(_privilegeMode) & 3);

        uint32_t mie = (mstatus >> 3) & 1;

        mstatus = (mstatus & ~((1u << 3) | (1u << 7) | (3u << 11))) | (mie << 7) | (mpp << 11);
        _csrUnit.write(CsrAddress::MSTATUS, mstatus);

        _privilegeMode = PrivilegeMode::Machine;
        _pc = _csrUnit.read(CsrAddress::MTVEC) & ~0x3;

        _nextPc = _pc;
    }
}

void RiscvCpu::returnFromTrap(PrivilegeMode retMode) {
    uint32_t mstatus = _csrUnit.read(CsrAddress::MSTATUS);

    if (retMode == PrivilegeMode::Supervisor) {
        _pc = _csrUnit.read(CsrAddress::SEPC);
        _nextPc = _pc;

        uint8_t previousPrivilege = (mstatus >> 8) & 1;
        _privilegeMode = static_cast<PrivilegeMode>(previousPrivilege);

        uint32_t spie_bit = (mstatus >> 5) & 1;
        uint32_t sie_bit = spie_bit << 1;

        mstatus = (mstatus & ~0x00000122) | sie_bit | (1 << 5);
        _csrUnit.write(CsrAddress::MSTATUS, mstatus);

    } else if (retMode == PrivilegeMode::Machine) {
        _pc = _csrUnit.read(CsrAddress::MEPC);
        _nextPc = _pc;

        uint8_t previousPrivilege = (mstatus >> 11) & 3;
        _privilegeMode = static_cast<PrivilegeMode>(previousPrivilege);

        uint32_t mpie_bit = (mstatus >> 7) & 1;
        uint32_t mie_bit = mpie_bit << 3;

        mstatus = (mstatus & ~0x00001888) | mie_bit | (1 << 7);
        _csrUnit.write(CsrAddress::MSTATUS, mstatus);
    }
}

void RiscvCpu::notifyStore(uint32_t address, uint32_t size) {
    if (!_reservationValid) {
        return;
    }
    const uint64_t storeStart = static_cast<uint64_t>(address);
    const uint64_t storeEnd   = storeStart + size;
    const uint64_t resStart   = static_cast<uint64_t>(_reservationAddress);
    const uint64_t resEnd     = resStart + 4ULL;

    if (storeStart < resEnd && storeEnd > resStart) {
        _reservationValid = false;
    }
}

void RiscvCpu::makeReservation(uint32_t physicalAddress) {
    _reservationAddress = physicalAddress;
    _reservationValid = true;
}

bool RiscvCpu::checkAndClearReservation(uint32_t physicalAddress) {
    bool isValid = _reservationValid && (_reservationAddress == physicalAddress);
    _reservationValid = false;
    return isValid;
}

void RiscvCpu::cancelReservation() {
    _reservationValid = false;
}

bool RiscvCpu::loadBinFileToMemory(const std::string& filename, uint32_t startAddr) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) return false;

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        return false;
    }

    for(size_t i = 0; i < buffer.size(); i++) {
        // TODO: consider loading bigger blocks of memory in the future
        _mem.write8(startAddr + i, buffer[i]);
    }

    return true;
}

void RiscvCpu::reset() {
    constexpr uint32_t ram_base = 0x80000000;
    constexpr uint32_t stack_size = 1024 * 1024;

    _privilegeMode = PrivilegeMode::Machine;

    _regs.fill(0);
    _pc = ram_base;
    _regs.at(2) = ram_base + stack_size;

    _mem.reset();
    _csrUnit.reset();

    cancelReservation();
}

void RiscvCpu::resetGUI() {
    _regs.fill(0);
    _pc = 0;
    _mem.reset();
}
