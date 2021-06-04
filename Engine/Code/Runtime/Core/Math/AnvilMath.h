#pragma once

#include "Types.h"

namespace Math {
static int32 const gLeadingBitTable[32]
  = {0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30, 8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31};

int32
GetLeadingBit(uint32 value)
{
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    uint32_t key = (value * 0x07C4ACDDu) >> 27;
    return gLeadingBitTable[key];
}

constexpr uint32
RoundUpToPowerOfTwo(uint32 value)
{
    if (value > 0) {
        int32  leading = GetLeadingBit(value);
        uint32 mask    = (1 << leading);
        if ((value & ~mask) == 0) {
            // value is a power of two
            return value;
        } else {
            // round up to a power of two
            return (mask << 1);
        }

    } else {
        return 1u;
    }
}
} // namespace Math