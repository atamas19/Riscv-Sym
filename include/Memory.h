#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>

#define MEMORY_SIZE 1024

// TODO: review all this Memory class, find out if it's right to be this way

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
    Memory() { memory.resize(MEMORY_SIZE, 0); }

    std::vector<int8_t> memory;
};

struct MemoryCell
{
    MemoryCell(uint32_t address, int32_t value): address(address), value(value) {};

    uint32_t address;
    int32_t value;
};
