#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>

class Ram
{
public:
    Ram(size_t size) { memory.resize(size, 0); }

    void write32(uint32_t address, uint32_t value);
    uint32_t read32(uint32_t address);

    void write16(uint32_t address, uint16_t value);
    uint16_t read16(uint32_t address);

    void write8(uint32_t address, uint8_t value);
    uint8_t read8(uint32_t address);
private:
    std::vector<uint8_t> memory;
};