#pragma once

#include <cstdint>
#include <vector>

#define MEMORY_SIZE INT32_MAX

class Memory {
public:
    static Memory& getInstance();

    void write32(uint32_t address, uint32_t value);
    uint32_t read32(uint32_t address);

    void write16(uint32_t address, uint16_t value);
    uint16_t read16(uint32_t address);

    void write8(uint32_t address, uint8_t value);
    uint8_t read8(uint32_t address);

    void reset();
private:
    Memory(): _memory(MEMORY_SIZE) {}

private:
    bool handleMMIO(uint32_t address, uint32_t value);

private:
    std::vector<uint8_t> _memory;
};

struct MemoryCell {
    MemoryCell(uint32_t address, uint32_t value): address(address), value(value) {}

    uint32_t address;
    uint32_t value;
};
