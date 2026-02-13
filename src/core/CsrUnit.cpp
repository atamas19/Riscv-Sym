#include "core/CsrUnit.h"

CsrUnit::CsrUnit() {
    reset();
}

void CsrUnit::reset() {
    _csrs.fill(0);
    _csrs[CsrAddress::MISA] = (1 << 8); 
    _csrs[CsrAddress::MHARTID] = 0;
}

uint32_t CsrUnit::read(uint16_t address) const {
    // TODO: check priviledge level

    if (address >= 4096) return 0;
    return _csrs[address];
}

void CsrUnit::write(uint16_t address, uint32_t value) {
    if (address >= 4096) return;

    if (isReadOnly(address)) {
        #if DEBUG
        std::cout << "[CSR] Warning: Attempted write to Read-Only CSR " 
                  << std::hex << address << std::dec << "\n";
        #endif
        return;
    }

    switch (address) {

        case CsrAddress::MSTATUS:
        {
            uint32_t mask = 0x000DE188;

            _csrs[address] = (_csrs[address] & ~mask) | (value & mask);
            break;
        }

        case CsrAddress::SATP:
        {
            // Aici e critic!
            // Formatul SATP: [MODE (1 bit)] [ASID (9 biți)] [PPN (22 biți)]
            
            _csrs[address] = value;
            
            // TODO: Aici vei apela o funcție din CPU/MMU:
            // mmu->flushTlb(); 
            // printf("Paginare schimbată! Mode: %d\n", value >> 31);
            break;
        }

        case CsrAddress::MISA: // 0x301
            // MISA este de obicei Read-Only în emulatoare simple.
            // Ignorăm scrierea.
            break;

        case CsrAddress::MIP: // 0x344 - Interrupt Pending
        {
            // Doar anumiți biți pot fi scriși software (ex: Software Interrupts)
            uint32_t mask = 0x00000333; 
            _csrs[address] = (_csrs[address] & ~mask) | (value & mask);
            
            // TODO: Verifică dacă trebuie să declanșezi o întrerupere imediat!
            break;
        }

        // ... alte registre ...

        default:
            // Pentru registrele generice (scratch), scriem direct
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

bool CsrUnit::isReadOnly(uint16_t address) const {
    // Biții 11 și 10 (0xC00) indică R/W vs RO
    // 00, 01, 10 -> R/W
    // 11 -> Read Only
    return (address >> 10) == 0b11;
}
