#pragma once

#include <memory>
#include <cstdint>
#include <array>

#include <core/instruction/Instruction.h>
#include <core/Memory.h>
#include <core/CsrUnit.h>

enum class ExceptionCause : uint32_t {
    InstructionAddressMisaligned = 0,
    InstructionAccessFault = 1,
    IllegalInstruction = 2,
    Breakpoint = 3,
    LoadAddressMisaligned = 4,
    LoadAccessFault = 5,
    StoreAddressMisaligned = 6,
    StoreAccessFault = 7,
    EnvironmentCallFromUMode = 8,
    EnvironmentCallFromSMode = 9,
    EnvironmentCallFromMMode = 11,
    InstructionPageFault = 12,
    LoadPageFault = 13,
    StorePageFault = 15
};

class RiscvCpu {
public:
    static RiscvCpu& getInstance();

    int executeAsmCommand(const std::string& command, InstructionOutput& instructionOutput);

    void reset();
    void resetGUI();

    bool executeFromBinFile(const std::string& filePath, uint32_t startAddr = 0x80000000);

    // Getters
    uint32_t getPc() const;
    uint32_t getRegister(uint8_t registerIndex) const;
    CsrUnit& getCsr();
    PrivilegeMode getPrivilegeMode() const;

    // Setters
    void setPc(uint32_t pcValue);
    void setRegister(uint8_t registerIndex, uint32_t registerValue);
    void setPrivilegeMode(PrivilegeMode mode);

public:
    void takeTrap(ExceptionCause cause, uint32_t trapValue = 0);
    void returnFromTrap(PrivilegeMode retMode);

public:
    void makeReservation(uint32_t address);
    bool checkAndClearReservation(uint32_t address);
    void cancelReservation();

private:
    RiscvCpu(): _mem(Memory::getInstance()) { reset(); };
    ~RiscvCpu() = default;
    RiscvCpu(const RiscvCpu&) = delete;
    RiscvCpu& operator=(const RiscvCpu&) = delete;
    RiscvCpu(RiscvCpu&&) = delete;
    RiscvCpu& operator=(RiscvCpu&&) = delete;

private:
    std::unique_ptr<Instruction> getInstructionFromAsmCommand(const std::string& asmCommand, InstructionOutput& instructionOutput);

    bool loadBinFileToMemory(const std::string& filename, uint32_t startAddr);

private:
    bool reservationValid = false;
    uint32_t reservationAddress = 0;

private:
    std::array<uint32_t, 32> _regs;
    uint32_t _pc;
    CsrUnit _csrUnit;
    Memory& _mem;

    PrivilegeMode _privilegeMode = PrivilegeMode::Machine;
};
