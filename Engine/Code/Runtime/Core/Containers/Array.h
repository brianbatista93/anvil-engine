#pragma once

#include "Memory/Allocators/DynamicAllocator.h"

/**
 * @brief Arrays of elements of contiguous allocated memory.
 * It can be dynamically resizable as well as with fixed size.
*/
template<class T, class Alloc = TDynamicAllocator<T>>
class TArray
{
  public:
    /**
     * @brief Default constructor.
    */
    constexpr explicit TArray() noexcept(noexcept(Alloc())) {}

    /**
     * @brief Copy constructor.
     * Constructs an array by coping the elements of other.
     * @param other The array to be copied.
    */
    constexpr TArray(const TArray& other)
    {
        m_allocator.Allocate(other.GetSize());
        MemoryUtils::CopyElements(GetData(), other.GetData(), other.GetSize());
    }

    /**
     * @brief Move constructor.
     * Constructs an array with the elements of other using move semantics.
     * @param other The array to be moved.
    */
    constexpr TArray(TArray&& other) {}

    /**
     * @brief Constructs a array with an initial slots count.
     * The slots are default initialized elements.
     * @param count The initial slots count.
    */
    constexpr TArray(uint32 count)
    {
        m_allocator.Allocate(count);
        MemoryUtils::ConstructElements(GetData(), GetSize());
    }

    ~TArray() {}

    constexpr T* GetData() { return m_allocator.GetData(); }

    constexpr const T* GetData() const { return m_allocator.GetData(); }

    /**
     * @brief Returns the number of elements present on the array.
     * @return Number of elements of the array.
    */
    constexpr uint32 GetSize() const { return m_allocator.GetSize(); }

    /**
     * @brief Returns the capacity of the array.
     * @return The capacity (in bytes) of the array.
    */
    constexpr uint32 GetCapacity() const { return m_allocator.GetCapacity(); }

    /**
     * @brief Checks if the array is empty.
     * @return True if the array is empty, false otherwise.
    */
    constexpr bool IsEmpty() const { return GetSize() == 0; }

    /**
     * @brief Returns an element at a position (index).
     * @param pos The position (index) of the desired element.
     * @return The element at the position pos.
    */
    constexpr T& operator[](uint32 pos)
    {
        FE_ASSERT(pos < GetSize());
        return GetData()[pos];
    }

    /**
     * @brief Returns an element at a position (index).
     * @param pos The position (index) of the desired element.
     * @return The element at the position pos.
    */
    constexpr const T& operator[](uint32 pos) const
    {
        FE_ASSERT(pos < GetSize());
        return GetData()[pos];
    }

  private:
    Alloc m_allocator;
};