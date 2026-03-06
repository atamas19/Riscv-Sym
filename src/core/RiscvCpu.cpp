#include <core/RiscvCpu.h>
#include <core/AssemblyCompiler.h>

#include <iostream>
#include <bitset>
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
        instruction->execute(*this, instructionOutput);
        instructionOutput.exitCode = 0;
    } catch(const std::exception& e) {
        instructionOutput.consoleLog = "Error executing `" + command + "`: " + std::string{e.what()};
        instructionOutput.exitCode = -1;
    }

    instruction.reset();

    return 0;
}

std::unique_ptr<Instruction> RiscvCpu::getInstructionFromAsmCommand(const std::string& asmCommand, InstructionOutput& instructionOutput) {
    uint32_t binaryInstruction = AssemblyCompiler::compile(asmCommand, instructionOutput);

    std::cout << "Binary Instruction: " << std::bitset<32>(binaryInstruction) << "\n";

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
        std::cout << "Couldn't load bin file to memory!\n";
        return false;
    }

    std::cout << "Instructions have been loaded. Starting execution...\n";

    this->_pc = startAddr;

    InstructionOutput test;

    for (int i{0}; true; ++i) {
        if (i % 20000 == 0) {
            _mem.pollKeyboard();
        }

        if (_mem.getUartInputChar() != -1) {
            PrivilegeMode currentMode = getPrivilegeMode();
            uint32_t sstatus = getCsr().read(0x100);
            bool sie = (sstatus & 0x2) != 0;

            if (currentMode == PrivilegeMode::User || (currentMode == PrivilegeMode::Supervisor && sie)) {

                takeTrap(static_cast<ExceptionCause>((uint32_t)0x80000009), 0);
                continue;
            }
        }

        try {
            #if DEBUG
            std::cout << "PC: " << std::to_string(this->_pc) << " executed for " << i << "\n";
            #endif
            uint32_t binaryInstruction = _mem.read32(this->_pc, true);

            if (binaryInstruction == 0 && this->_pc == 0) {
                 std::cout << "Halted at PC=0\n"; break;
            }

            auto instruction = InstructionFactory::create(binaryInstruction);
            if (!instruction) {
                takeTrap(ExceptionCause::IllegalInstruction, binaryInstruction);
                continue;
            }

            instruction->execute(*this, test);

            #if DEBUG
            std::cout << "\nConsole log: " << test.consoleLog << "\n";
            #endif

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
    uint32_t causeCode = static_cast<uint32_t>(cause);

    bool delegateToS = (_privilegeMode <= PrivilegeMode::Supervisor) &&
                       ((_csrUnit.read(CsrAddress::MEDELEG) & (1 << causeCode)) != 0);

    if (delegateToS) {
        _csrUnit.write(CsrAddress::SEPC, _pc);

        _csrUnit.write(CsrAddress::SCAUSE, causeCode);
        _csrUnit.write(CsrAddress::STVAL, trapValue);

        uint32_t mstatus = _csrUnit.read(CsrAddress::MSTATUS);
        uint32_t spp_bit = (static_cast<uint32_t>(_privilegeMode) & 1) << 8;

        uint32_t sie_bit = (mstatus >> 1) & 1;
        uint32_t spie_bit = sie_bit << 5;

        mstatus = (mstatus & ~0x00000122) | spp_bit | spie_bit;
        _csrUnit.write(CsrAddress::MSTATUS, mstatus);

        _privilegeMode = PrivilegeMode::Supervisor;

        _pc = _csrUnit.read(CsrAddress::STVEC) & ~0x3;

    } else {
        _csrUnit.write(CsrAddress::MEPC, _pc);
        _csrUnit.write(CsrAddress::MCAUSE, causeCode);
        _csrUnit.write(CsrAddress::MTVAL, trapValue);

        uint32_t mstatus = _csrUnit.read(CsrAddress::MSTATUS);

        uint32_t mpp_bits = (static_cast<uint32_t>(_privilegeMode) & 3) << 11;

        uint32_t mie_bit = (mstatus >> 3) & 1;
        uint32_t mpie_bit = mie_bit << 7;

        mstatus = (mstatus & ~0x00001888) | mpp_bits | mpie_bit;
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
}

void RiscvCpu::resetGUI() {
    _regs.fill(0);
    _pc = 0;
    _mem.reset();
}
