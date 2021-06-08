#pragma once

#include "Memory/MemoryUtils.h"

struct AllocatorItem
{};

template<class _SizeType>
class THeapAllocator
{
  public:
    using SizeType = _SizeType;

    /**
     * @brief Default constructor.
    */
    constexpr THeapAllocator()
      : m_data(nullptr)
    {}

    /**
     * @brief Move constructor.
     * @param other 
    */
    constexpr THeapAllocator(THeapAllocator&& other)
    {
        m_data       = other.m_data;
        other.m_data = nullptr;
    }

    /** Destructor.*/
    ~THeapAllocator()
    {
        if (m_data) {
            MemoryUtils::FreeAligned(m_data);
            m_data = nullptr;
        }
    }

    /**
     * @brief Move assignment
     * Be sure to destroy all items before.
     * @param other 
    */
    constexpr THeapAllocator& operator=(THeapAllocator&& other)
    {
        AE_ASSERT((void*)this != (void*)&other);

        if (m_data) {
            MemoryUtils::FreeAligned(m_data);
        }

        m_data       = other.m_data;
        other.m_data = nullptr;

        return *this;
    }

    /**
     * @brief Reallocate data to fit a count of items.
     * @param count Number of items.
     * @param itemSizeInBytes Size in bytes of a single item.
    */
    void Reallocate(SizeType count, size_t itemSizeInBytes)
    {
        if (m_data || count) {
            m_data = reinterpret_cast<AllocatorItem*>(MemoryUtils::ReallocateAligned(m_data, count * itemSizeInBytes));
        }
    }

    constexpr SizeType CalculateGrowth(SizeType newItemsCount, SizeType currItemsCount) const
    {
        AE_ASSERT(newItemsCount > currItemsCount && newItemsCount > 0);

        size_t result = 4;

        if (currItemsCount) {
            result = size_t(newItemsCount) + 3 * size_t(newItemsCount) / 8 + 16;
        } else if (size_t(newItemsCount) > result) {
            result = size_t(newItemsCount);
        }

        return result;
    }

    constexpr SizeType CalculateReserve(SizeType itemsCount) const { return itemsCount; }

    /**
     * @brief Get allocated data.
     * @return Pointer to allocated data.
    */
    constexpr AllocatorItem* GetData() const { return m_data; }

    /**
     * @brief Checks if there is allocated data.
     * @return True if there is allocated data else otherwise.
    */
    constexpr bool HasAllocatedData() const { return !!m_data; }

  private:
    THeapAllocator(const THeapAllocator&);
    THeapAllocator& operator=(const THeapAllocator&);

    AllocatorItem* m_data;
};