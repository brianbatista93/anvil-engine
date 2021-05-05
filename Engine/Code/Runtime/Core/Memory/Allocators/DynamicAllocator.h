#pragma once

#include "Memory/MemoryUtils.h"

/**
 * @brief Allocates contiguous resizable memory.
*/
template<class T>
class TDynamicAllocator
{
  public:
    using ValueType = T;
    using SizeType  = uint64;

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

        other.m_first {};
        other.m_last {};
        other.m_end {};
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
    ValueType* Allocate(SizeType count)
    {
        if (GetSize() + count > GetCapacity()) {
            Reserve(GetSize() + count);
        }

        const SizeType offset = GetSize();
        m_last += count;

        return m_first + offset;
    }

    constexpr void Deallocate(T* ptr, SizeType count)
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
    constexpr SizeType GetSize() const { return m_last - m_first; }

    /**
     * @brief Returns the max number of elements the allocator can allocate.
     * @return The max number of elements the allocator can allocate.
    */
    constexpr SizeType GetCapacity() const { return m_end - m_first; }

    constexpr ValueType* GetData() { return m_first; }

    constexpr const ValueType* GetData() const { return m_first; }

    /**
     * @brief C++ iterator begin.
     * @return A pointer to the begin.
    */
    constexpr ValueType* begin() noexcept { return m_first; }

    /**
     * @brief C++ iterator begin.
     * @return A pointer to the begin.
    */
    constexpr const ValueType* begin() const noexcept { return m_first; }

    /**
     * @brief C++ iterator end.
     * @return A pointer to the end.
    */
    constexpr ValueType* end() noexcept { return m_last; }

    /**
     * @brief C++ iterator end.
     * @return A pointer to the end.
    */
    constexpr const ValueType* end() const noexcept { return m_last; }

    constexpr void Destroy()
    {
        if (GetCapacity()) {
            m_end  = nullptr;
            m_last = nullptr;
            MemoryUtils::FreeAligned(m_first);
            m_first = nullptr;
        }
    }

    constexpr void Pop(uint32 count = 1)
    {
        AE_ASSERT(m_last);
        m_last -= count;
    }

  private:
    constexpr void AllocateMemoryBlock(SizeType newSize)
    {
        const SizeType oldSize     = GetSize();
        const SizeType currentSize = newSize;
        const uint64   actualSize  = static_cast<uint64>(currentSize * sizeof(ValueType));
        ValueType*     temp        = reinterpret_cast<ValueType*>(MemoryUtils::AllocateAligned(actualSize));
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
    ValueType* m_first;
    ValueType* m_last;
    ValueType* m_end;
};