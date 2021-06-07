#pragma once

#include "Types.h"

/**
 * @brief Collection of memory utils operations.
*/
class MemoryUtils
{
  public:
    /**
     * @brief Aligns the given address to the given align size.
     * @param address Memory address to be aligned.
     * @param align Required alignment.
     * @return Aligned address.
    */
    static constexpr uint64 AlignAddress(uint64 address, uint64 align)
    {
        const uint64 mask = align - 1;
        AE_ASSERT((align & mask) == 0);
        return (address + mask) & ~mask;
    }

    /**
     * @brief Aligns the given pointer to the given align size.
     * @tparam T 
     * @param ptr Pointer to be aligned.
     * @param align Required alignment.
     * @return Aligned pointer.
    */
    template<class T>
    static constexpr T* AlignPointer(T* ptr, uint64 align)
    {
        const uint64 address        = reinterpret_cast<uint64>(ptr);
        const uint64 addressAligned = AlignAddress(address, align);
        return reinterpret_cast<T*>(addressAligned);
    }

    /**
     * @brief Allocates an aligned memory block.
     * @param size Size in bytes of the memory block
     * @param align Desired alignment.
     * @return Pointer to the allocated memory.
    */
    static void* AllocateAligned(uint64 size, uint64 align = 16u);

    /**
     * @brief Free an aligned memory block.
     * @param memoryBlock Pointer to memory block to be freed.
    */
    static void FreeAligned(void* memoryBlock);

    template<class T>
    static constexpr void CopyElements(T* dst, const T* src, uint64 size)
    {
        AE_ASSERT(dst);
        AE_ASSERT(src);
        AE_ASSERT(size);

        if constexpr (std::is_trivially_constructible<T>::value) {
            memcpy(dst, src, size * sizeof(T));
        } else {
            for (uint64 i = 0; i < size; i++) {
                dst[i] = src[i];
            }
        }
    }

    template<class T, class... Args>
    static constexpr void ConstructElements(T* dst, uint64 count, Args&&... args)
    {
        AE_ASSERT(dst);

        if constexpr (std::is_constructible<T>::value) {
            for (uint64 i = 0; i < count; i++) {
                dst[i] = T(std::forward<Args>(args)...);
            }
        }
    }

    template<class ItemType, class SizeType>
    static constexpr void DestroyElements(ItemType* item, SizeType count)
    {
        if constexpr (!std::is_trivially_destructible_v<ItemType>) {
            while (count) {
                class DestructItemsElementType = ItemType;

                item->DestructItemsElementType::~DestructItemsElementType();
                ++item;
                --count;
            }
        }
    }

    static void CopyMemory(void* dst, const void* begin, uint64 size) { memcpy(dst, begin, size); }

    static void CopyMemory(void* dst, const void* begin, const void* end)
    {
        AE_ASSERT(reinterpret_cast<uint64>(end) > reinterpret_cast<uint64>(begin));
        const uint64 size = reinterpret_cast<uint64>(end) - reinterpret_cast<uint64>(begin);

        CopyMemory(dst, begin, size);
    }
};