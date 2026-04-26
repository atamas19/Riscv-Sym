#include <core/RiscvCpu.h>
#include <core/AssemblyCompiler.h>

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
    auto instruction = getInstructionFromAsmCommand(command, instructionOutput);

    if (instruction == nullptr) {
        instructionOutput.exitCode = -1;
        return 1;
    }

    try {
        instruction->execute(*this, &instructionOutput);
        instructionOutput.exitCode = 0;
    } catch(const std::exception& e) {
        instructionOutput.consoleLog = "Error executing `" + command + "`: " + std::string{e.what()};
        instructionOutput.exitCode = -1;
    }

    return 0;
}

std::unique_ptr<Instruction> RiscvCpu::getInstructionFromAsmCommand(const std::string& asmCommand, InstructionOutput& instructionOutput) {
    uint32_t binaryInstruction = AssemblyCompiler::compile(asmCommand, &instructionOutput);

    spdlog::debug("Binary Instruction: {:032b}", binaryInstruction);

    if (binaryInstruction == 0) {
        instructionOutput.consoleLog = "Error converting `" + asmCommand + "` to binary: " + instructionOutput.consoleLog;
        return nullptr;
    }

    auto instruction = InstructionFactory::create(binaryInstruction);

    if (instruction == nullptr)
        instructionOutput.consoleLog = asmCommand + " doesn't exist";
        // Should never come into this if statement

    return instruction;
}

bool RiscvCpu::executeFromBinFile(const std::string& filePath, uint32_t startAddr) {
    if (!loadBinFileToMemory(filePath, startAddr)) {
        spdlog::error("Couldn't load bin file to memory!");
        return false;
    }

    spdlog::info("Instructions have been loaded. Starting execution...");

    this->_pc = startAddr;

    for (int i{0}; true; ++i) {
        if (i % 20000 == 0) {
            _mem.pollKeyboard();
        }

        if (_mem.getUartInputChar() != -1) {
            PrivilegeMode currentMode = getPrivilegeMode();

            if (currentMode != PrivilegeMode::Machine) {
                uint32_t sstatus = getCsr().read(0x100);
                uint32_t sieCsr = getCsr().read(0x104);

                bool sieGlobal = (sstatus & 0x2) != 0;
                bool seie = (sieCsr & (1u << 9)) != 0;

                if (seie && (currentMode == PrivilegeMode::User ||
                            (currentMode == PrivilegeMode::Supervisor && sieGlobal))) {
                    uint32_t sipCsr = getCsr().read(0x144);
                    sipCsr |= (1u << 9);
                    getCsr().write(0x144, sipCsr);

                    takeTrap(static_cast<ExceptionCause>((uint32_t)0x80000009), 0);
                    continue;
                }
            }
        }

        try {
            uint32_t binaryInstruction = _mem.read32(this->_pc, true);

            if (binaryInstruction == 0 && this->_pc == 0) {
                spdlog::info("Halted at PC=0");
                break;
            }

            auto instruction = InstructionFactory::create(binaryInstruction);
            if (!instruction) {
                takeTrap(ExceptionCause::IllegalInstruction, binaryInstruction);
                continue;
            }

            instruction->execute(*this);

        } catch (const PageFaultException& e) {
            ExceptionCause cause;
            if (e.accessType == AccessType::InstructionFetch) {
                cause = ExceptionCause::InstructionPageFault;
            } else if (e.accessType == AccessType::Load) {
                cause = ExceptionCause::LoadPageFault;
            } else {
                cause = ExceptionCause::StorePageFault;
            }

            takeTrap(cause, e.faultingAddress);
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
        _csrUnit.write(CsrAddress::SEPC, _pc);
        _csrUnit.write(CsrAddress::SCAUSE, rawCause);
        _csrUnit.write(CsrAddress::STVAL, trapValue);

        uint32_t sstatus = _csrUnit.read(CsrAddress::SSTATUS);
        uint32_t spp = (static_cast<uint32_t>(_privilegeMode) & 1);
        uint32_t sie = (sstatus >> 1) & 1;

        sstatus = (sstatus & ~((1u << 1) | (1u << 5) | (1u << 8))) | (sie << 5) | (spp << 8);
        _csrUnit.write(CsrAddress::SSTATUS, sstatus);

        _privilegeMode = PrivilegeMode::Supervisor;
        _pc = _csrUnit.read(CsrAddress::STVEC) & ~0x3;

    } else {
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
    }
}

void RiscvCpu::returnFromTrap(PrivilegeMode retMode) {
    uint32_t mstatus = _csrUnit.read(CsrAddress::MSTATUS);

    if (retMode == PrivilegeMode::Supervisor) {
        _pc = _csrUnit.read(CsrAddress::SEPC);

        uint8_t previousPrivilege = (mstatus >> 8) & 1;
        _privilegeMode = static_cast<PrivilegeMode>(previousPrivilege);

        uint32_t spie_bit = (mstatus >> 5) & 1;
        uint32_t sie_bit = spie_bit << 1;

        mstatus = (mstatus & ~0x00000122) | sie_bit | (1 << 5);
        _csrUnit.write(CsrAddress::MSTATUS, mstatus);

    } else if (retMode == PrivilegeMode::Machine) {
        _pc = _csrUnit.read(CsrAddress::MEPC);

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
