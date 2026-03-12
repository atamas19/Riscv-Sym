#include "core/CsrUnit.h"

#include <spdlog/spdlog.h>

namespace {
    // --- Bits for mstatus ---
    constexpr uint32_t MSTATUS_SIE  = 1 << 1;  // Supervisor Interrupt Enable
    constexpr uint32_t MSTATUS_MIE  = 1 << 3;  // Machine Interrupt Enable
    constexpr uint32_t MSTATUS_SPIE = 1 << 5;  // Supervisor Previous Interrupt Enable
    constexpr uint32_t MSTATUS_MPIE = 1 << 7;  // Machine Previous Interrupt Enable
    constexpr uint32_t MSTATUS_SPP  = 1 << 8;  // Supervisor Previous Privilege
    constexpr uint32_t MSTATUS_MPP  = 3 << 11; // Machine Previous Privilege (2 bits)
    constexpr uint32_t MSTATUS_FS   = 3 << 13; // FPU Status (2 bits)
    constexpr uint32_t MSTATUS_SUM  = 1 << 18; // Supervisor User Memory Access

    constexpr uint32_t MSTATUS_MASK = MSTATUS_SIE | MSTATUS_MIE | MSTATUS_SPIE |
                                      MSTATUS_MPIE | MSTATUS_SPP | MSTATUS_MPP |
                                      MSTATUS_FS | MSTATUS_SUM;

    constexpr uint32_t SSTATUS_MASK = MSTATUS_SIE | MSTATUS_SPIE | MSTATUS_SPP |
                                      MSTATUS_FS | MSTATUS_SUM;

    // --- Bits for misa ---
    constexpr uint32_t MISA_RV32 = 1 << 30; // 01 for RV32
    constexpr uint32_t MISA_I    = 1 << 8;  // Base Integer ISA
    constexpr uint32_t MISA_M    = 1 << 12; // Integer Multiply/Divide
    constexpr uint32_t MISA_A    = 1 << 0;  // Atomic
    constexpr uint32_t MISA_S    = 1 << 18; // Supervisor mode
    constexpr uint32_t MISA_U    = 1 << 20; // User mode
}

CsrUnit::CsrUnit() {
    reset();
}

void CsrUnit::reset() {
    _csrs.fill(0);

    _csrs[CsrAddress::MISA] = MISA_RV32 | MISA_I | MISA_M | MISA_A | MISA_S | MISA_U;
    _csrs[CsrAddress::MSTATUS] = MSTATUS_MPP;
    _csrs[CsrAddress::MHARTID] = 0;
}

uint32_t CsrUnit::read(uint16_t address) const {
    if (address >= 4096) return 0;

    if (address == 0xC00) { // CYCLE (Low 32 bits)
        static uint32_t fake_time = 0;
        fake_time += 10000;
        return fake_time;
    }
    if (address == 0xC80) { // CYCLEH (High 32 bits)
        return 0;
    }

    switch (address) {
        case CsrAddress::SSTATUS:
            return _csrs[CsrAddress::MSTATUS] & SSTATUS_MASK;

        case CsrAddress::SIE:
            return _csrs[CsrAddress::MIE] & _csrs[CsrAddress::MIDELEG];

        case CsrAddress::SIP:
            return _csrs[CsrAddress::MIP] & _csrs[CsrAddress::MIDELEG];

        default:
            return _csrs[address];
    }
}

void CsrUnit::write(uint16_t address, uint32_t value) {
    if (address >= 4096) return;

    if (isReadOnly(address)) {
        spdlog::debug("[CSR] Attempted write to Read-Only CSR 0x{:x}", address);
        return;
    }

    switch (address) {
        // --- M-Mode Status ---
        case CsrAddress::MSTATUS: {
            uint32_t val = value & MSTATUS_MASK;

            if ((val & MSTATUS_MPP) == (2 << 11)) {
                val &= ~MSTATUS_MPP;
            }

            _csrs[address] = val;
            break;
        }

        // --- S-Mode Status (Alias spre MSTATUS) ---
        case CsrAddress::SSTATUS: {
            uint32_t current_mstatus = _csrs[CsrAddress::MSTATUS];
            _csrs[CsrAddress::MSTATUS] = (current_mstatus & ~SSTATUS_MASK) | (value & SSTATUS_MASK);
            break;
        }

        // --- Delegare S-Mode ---
        case CsrAddress::SIE: {
            uint32_t current_mie = _csrs[CsrAddress::MIE];
            uint32_t deleg_mask = _csrs[CsrAddress::MIDELEG];
            _csrs[CsrAddress::MIE] = (current_mie & ~deleg_mask) | (value & deleg_mask);
            break;
        }

        case CsrAddress::SIP: {
            uint32_t current_mip = _csrs[CsrAddress::MIP];
            uint32_t deleg_mask = _csrs[CsrAddress::MIDELEG];
            uint32_t writable_mask = deleg_mask & 0x0222;
            _csrs[CsrAddress::MIP] = (current_mip & ~writable_mask) | (value & writable_mask);
            break;
        }

        case CsrAddress::MIP: {
            uint32_t mask = 0x0222;
            _csrs[address] = (_csrs[address] & ~mask) | (value & mask);
            break;
        }

        case CsrAddress::SATP: {
            _csrs[address] = value;
            // TODO: Here the MMU will need to flush the TLB
            break;
        }

        // --- Hardware Read-Only Registers ---
        case CsrAddress::MISA:
        case CsrAddress::MHARTID:
        case CsrAddress::MVENDORID:
        case CsrAddress::MARCHID:
        case CsrAddress::MIMPID:
            break;

        default:
            _csrs[address] = value;
            break;
    }
}

void CsrUnit::setBit(uint16_t address, uint32_t bitMask) {
    uint32_t val = read(address);
    write(address, val | bitMask);
}

void CsrUnit::clearBit(uint16_t address, uint32_t bitMask) {
    uint32_t val = read(address);
    write(address, val & ~bitMask);
}

bool CsrUnit::canAccess(uint16_t address, PrivilegeMode currentMode, bool isWrite) const {
    uint8_t minPrivilegeRequired = (address >> 8) & 0x3;

    if (static_cast<uint8_t>(currentMode) < minPrivilegeRequired) {
        return false;
    }

    if (isWrite && isReadOnly(address)) {
        return false;
    }

    return true;
}

bool CsrUnit::isReadOnly(uint16_t address) const {
    return (address >> 10) == 0b11;
}
