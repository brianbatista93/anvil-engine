#include "MemoryUtils.h"

void*
MemoryUtils::AllocateAligned(uint64 size, uint64 align)
{
    uint64 actualSize = size + align;

    uint8* rawMemory = new uint8[actualSize];

    uint8* alignedMemory = AlignPointer(rawMemory, align);
    if (alignedMemory == rawMemory) {
        alignedMemory += align;
    }

    int64 shift = alignedMemory - rawMemory;
    AE_ASSERT(shift > 0 && shift <= 256);

    alignedMemory[-1] = static_cast<uint8>(shift & 0xFF);

    return alignedMemory;
}

void
MemoryUtils::FreeAligned(void* memoryBlock)
{
    AE_ASSERT(memoryBlock);
    uint8* alignedMemory = reinterpret_cast<uint8*>(memoryBlock);

    int64 shift = alignedMemory[-1];
    if (shift == 0) {
        shift = 256;
    }

    uint8* rawMemory = alignedMemory - shift;
    delete[] rawMemory;
}
