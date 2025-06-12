#pragma once

#include <cstdint>
#include <array>

#define MEMORY_SIZE 4096 // 4 kb

class Memory
{
public:
    static Memory& getInstance();

    void write32(uint32_t address, int32_t value);
    int32_t read32(uint32_t address);

    void write16(uint32_t address, int16_t value);
    int16_t read16(uint32_t address);

    void write8(uint32_t address, int8_t value);
    int8_t read8(uint32_t address);

    void reset();
private:
    Memory() { reset(); }

    std::array<int8_t, MEMORY_SIZE> memory;
};

struct MemoryCell
{
    MemoryCell(uint32_t address, int32_t value): address(address), value(value) {};

    uint32_t address;
    int32_t value;
};
