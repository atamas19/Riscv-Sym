#pragma once

#include <array>
#include <cstdint>
#include <iostream>

namespace CsrAddress {
    // --- Machine Level (M-Mode) ---
    constexpr uint16_t MSTATUS   = 0x300; // Machine Status
    constexpr uint16_t MISA      = 0x301; // ISA and Extensions
    constexpr uint16_t MEDELEG   = 0x302; // Exception Delegation
    constexpr uint16_t MIDELEG   = 0x303; // Interrupt Delegation
    constexpr uint16_t MIE       = 0x304; // Machine Interrupt Enable
    constexpr uint16_t MTVEC     = 0x305; // Machine Trap-Vector Base-Address
    constexpr uint16_t MCOUNTEREN= 0x306; // Machine Counter Enable

    constexpr uint16_t MEPC      = 0x341; // Machine Exception Program Counter
    constexpr uint16_t MCAUSE    = 0x342; // Machine Cause
    constexpr uint16_t MTVAL     = 0x343; // Machine Trap Value
    constexpr uint16_t MIP       = 0x344; // Machine Interrupt Pending

    // --- Supervisor Level (S-Mode) ---
    constexpr uint16_t SSTATUS   = 0x100; // Supervisor Status
    constexpr uint16_t SIE       = 0x104; // Supervisor Interrupt Enable
    constexpr uint16_t STVEC     = 0x105; // Supervisor Trap Vector
    constexpr uint16_t SEPC      = 0x141; // Supervisor Exception PC
    constexpr uint16_t SCAUSE    = 0x142; // Supervisor Cause
    constexpr uint16_t STVAL     = 0x143; // Supervisor Trap Value
    constexpr uint16_t SIP       = 0x144; // Supervisor Interrupt Pending
    constexpr uint16_t SATP      = 0x180; // Supervisor Address Translation and Protection (MMU!)

    // --- User Level / Info ---
    constexpr uint16_t CYCLE     = 0xC00;
    constexpr uint16_t TIME      = 0xC01;
    constexpr uint16_t INSTRET   = 0xC02;
    constexpr uint16_t MVENDORID = 0xF11;
    constexpr uint16_t MARCHID   = 0xF12;
    constexpr uint16_t MIMPID    = 0xF13;
    constexpr uint16_t MHARTID   = 0xF14; // Hardware Thread ID
}

class CsrUnit {
public:
    CsrUnit();

    void reset();

    uint32_t read(uint16_t address) const;

    void write(uint16_t address, uint32_t value);

    void setBit(uint16_t address, uint32_t bitMask);
    void clearBit(uint16_t address, uint32_t bitMask);

private:
    std::array<uint32_t, 4096> _csrs;

    bool isReadOnly(uint16_t address) const;
};
