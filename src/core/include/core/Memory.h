#pragma once

#include <cstdint>
#include <unordered_map>
#include <array>
#include <vector>
#include <memory>

class Memory {
public:
    static constexpr uint32_t UART_ADDR        = 0x10000000;
    static constexpr uint32_t SYSCTRL_ADDR     = 0x10000020;
    static constexpr uint32_t SYSCTRL_EXIT_OK  = 0x5555;
    static constexpr uint32_t SYSCTRL_EXIT_ERR = 0x3333;

    static constexpr uint32_t PAGE_SIZE = 4096;
    static constexpr uint32_t PAGE_MASK = PAGE_SIZE - 1;
    static constexpr uint32_t PAGE_SHIFT = 12;

public:
    static Memory& getInstance();

    void loadDiskImage(const std::string& path);

    // --- Controlul MMU ---
    // Chemat de RiscvCpu de fiecare dată când valoarea din registrul SATP se schimbă
    void setSATP(uint32_t satp);

    void write32(uint32_t address, uint32_t value);
    uint32_t read32(uint32_t address);

    void write16(uint32_t address, uint16_t value);
    uint16_t read16(uint32_t address);

    void write8(uint32_t address, uint8_t value);
    uint8_t read8(uint32_t address);

    void reset();

private:
    bool handleMMIO(uint32_t address, uint32_t value);
    bool handleMMIORead(uint32_t address, uint8_t& outValue);

    uint8_t* getMemoryPtr(uint32_t address, bool allocateIfNeeded);
    
    // --- MMU (Paginare Sv32) ---
    uint32_t translateAddress(uint32_t vaddr);
    uint32_t read32Physical(uint32_t paddr); // Citire brută, folosită de MMU pentru Page Tables

private:
    using Page = std::array<uint8_t, PAGE_SIZE>;
    std::unordered_map<uint32_t, std::unique_ptr<Page>> _pages;

    std::vector<uint8_t> _disk;
    
    uint32_t _currentSatp = 0; // Păstrăm starea MMU-ului

    int _spiState = 0;             
    uint8_t _spiCmd = 0;
    uint32_t _spiArg = 0;          
    int _spiArgBytesReceived = 0;  
    int _spiDataBytesTransferred = 0;
    uint8_t _spiReadBuffer = 0xFF;
    uint16_t _spiCurrentCrc = 0;
    int _uartInputChar = -1;
};

struct MemoryCell {
    MemoryCell(uint32_t address, uint32_t value): address(address), value(value) {}

    uint32_t address;
    uint32_t value;
};
