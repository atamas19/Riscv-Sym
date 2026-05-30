#pragma once

#include <cstdint>

namespace CType
{
    // Decompress a 16-bit RVC (compressed) instruction into a 32-bit
    // standard RISC-V encoded instruction. Returns 0 for illegal or
    // unimplemented compressed encodings.
    uint32_t decompress(uint16_t compressedInstruction);

} // namespace CType
