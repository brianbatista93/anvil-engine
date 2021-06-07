#pragma once

#include "Memory/MemoryUtils.h"

/**
 * @brief Allocates contiguous resizable memory.
*/
template<class _ItemType, class _SizeType>
class TDynamicAllocator
{
  public:
    using ItemType = _ItemType;
    using SizeType = _SizeType;

    /**
     * @brief Default contructor.
    */
    constexpr TDynamicAllocator() noexcept {}

    constexpr TDynamicAllocator(const TDynamicAllocator& other) noexcept = default;

    constexpr TDynamicAllocator(TDynamicAllocator&& other) noexcept
      : m_first(other.m_first)
      , m_last(other.m_last)
      , m_end(other.m_end)
    {
        other.m_first = nullptr;
        other.m_last  = nullptr;
        other.m_end   = nullptr;
    }

    constexpr TDynamicAllocator& operator=(const TDynamicAllocator& other) noexcept = default;

    constexpr TDynamicAllocator& operator=(TDynamicAllocator&& other) noexcept
    {
        m_first = other.m_first;
        m_last  = other.m_last;
        m_end   = other.m_end;

        other.m_first = nullptr;
        other.m_last  = nullptr;
        other.m_end   = nullptr;

        return *this;
    }

    /**
     * @brief Reserves a count of elements in the memory.
     * @param count The number of elements to reserve in memory.
    */
    void Reserve(SizeType count)
    {
        AE_ASSERT(count > 0);

        if (count > GetCapacity()) {
            AllocateMemoryBlock(count);
        }
    }

    /**
     * @brief Allocates a number of elements on the allocator.
     * @param count The number of elements.
     * @return Pointer to the allocated elements.
    */
    ItemType* Allocate(SizeType count)
    {
        if (GetSize() + count > GetCapacity()) {
            Reserve(GetSize() + count);
        }

        const SizeType offset = GetSize();
        m_last += count;

        return m_first + offset;
    }

    constexpr void Deallocate(ItemType* ptr, SizeType count)
    {
        m_first = nullptr;
        m_last  = nullptr;
        m_end   = nullptr;
        if (m_first != m_end) {
            MemoryUtils::FreeAligned(ptr);
        }
    }

    constexpr void Reset(SizeType cursor = 0) { m_last = m_first + cursor; }

    /**
     * @brief Returns the number of elements allocated on the allocator.
     * @return The number of elements allocated on the allocator.
    */
    constexpr SizeType GetSize() const { return static_cast<SizeType>(m_last - m_first); }

    /**
     * @brief Returns the max number of elements the allocator can allocate.
     * @return The max number of elements the allocator can allocate.
    */
    constexpr SizeType GetCapacity() const { return static_cast<SizeType>(m_end - m_first); }

    constexpr ItemType* GetData() { return m_first; }

    constexpr const ItemType* GetData() const { return m_first; }

    /**
     * @brief C++ iterator begin.
     * @return A pointer to the begin.
    */
    constexpr ItemType* begin() noexcept { return m_first; }

    /**
     * @brief C++ iterator begin.
     * @return A pointer to the begin.
    */
    constexpr const ItemType* begin() const noexcept { return m_first; }

    /**
     * @brief C++ iterator end.
     * @return A pointer to the end.
    */
    constexpr ItemType* end() noexcept { return m_last; }

    /**
     * @brief C++ iterator end.
     * @return A pointer to the end.
    */
    constexpr const ItemType* end() const noexcept { return m_last; }

    constexpr void Destroy()
    {
        MemoryUtils::DestroyElements(m_first, GetSize());
        if (GetCapacity()) {
            m_end  = nullptr;
            m_last = nullptr;
            MemoryUtils::FreeAligned(m_first);
            m_first = nullptr;
        }
    }

    constexpr void Pop(SizeType count = 1)
    {
        AE_ASSERT(m_last);
        m_last -= count;
    }

  private:
    constexpr void AllocateMemoryBlock(SizeType newSize)
    {
        const SizeType oldSize     = GetSize();
        const SizeType currentSize = newSize;
        const uint64   actualSize  = static_cast<uint64>(currentSize * sizeof(ItemType));
        ItemType*      temp        = reinterpret_cast<ItemType*>(MemoryUtils::AllocateAligned(actualSize));
        if (oldSize > 0) {
            MemoryUtils::CopyElements(temp, m_first, GetSize());
            MemoryUtils::FreeAligned(m_first);
        }
        m_first = temp;
        temp    = nullptr;

        m_last = m_first + oldSize;
        m_end  = m_first + currentSize;
    }

  private:
    ItemType* m_first = nullptr;
    ItemType* m_last  = nullptr;
    ItemType* m_end   = nullptr;
};