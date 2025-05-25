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

    void write32(uint32_t address, uint32_t value);
    uint32_t read32(uint32_t address);

    void write16(uint32_t address, uint16_t value);
    uint16_t read16(uint32_t address);

    void write8(uint32_t address, uint8_t value);
    uint8_t read8(uint32_t address);

    void reset();
private:
    Memory() { memory.resize(MEMORY_SIZE, 0); }

    std::vector<uint8_t> memory;
};
