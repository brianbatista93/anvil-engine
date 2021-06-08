#pragma once

#include "ContainerAllocators.h"

/**
 * @brief Arrays of elements of contiguous allocated memory.
 * It can be dynamically resizable as well as with fixed size.
*/
template<class _ItemType, class _AllocType>
class TArray
{
  public:
    using ItemType      = _ItemType;
    using AllocatorType = _AllocType;
    using SizeType      = typename _AllocType::SizeType;

    /**
     * @brief Default constructor.
    */
    constexpr explicit TArray()
      : m_size(0)
      , m_capacity(0)
    {}

    /**
     * @brief Copy constructor.
     * Constructs an array by coping the elements of other.
     * @param other The array to be copied.
    */
    constexpr TArray(const TArray& other)
    {
        m_size     = other.m_size;
        m_capacity = other.m_capacity;

        m_allocator.Reallocate(other.m_size, sizeof(ItemType));
        MemoryUtils::CopyElements(GetData(), other.GetData(), other.GetSize());
    }

    /**
     * @brief Move constructor.
     * Constructs an array with the elements of other using move semantics.
     * @param other The array to be moved.
    */
    constexpr TArray(TArray&& other)
    {
        m_size     = other.m_size;
        m_capacity = other.m_capacity;

        m_allocator = std::move(other.m_allocator);

        other.m_size     = 0;
        other.m_capacity = 0;
    }

    /**
     * @brief Constructs a array with an initial slots count.
     * The slots are default initialized elements.
     * @param count The initial slots count.
    */
    constexpr TArray(SizeType count)
    {
        m_size     = count;
        m_capacity = count;

        m_allocator.Reallocate(m_size, sizeof(ItemType));

        MemoryUtils::ConstructElements(GetData(), m_size);
    }

    /**
     * @brief Initializer List constructor.
     * @param list List to be copied to the new created array.
    */
    constexpr TArray(std::initializer_list<ItemType> list)
    {
        const SizeType size = static_cast<SizeType>(list.size());
        if (size) {
            m_size     = size;
            m_capacity = size;

            m_allocator.Reallocate(m_size, sizeof(ItemType));
            MemoryUtils::CopyElements(GetData(), list.begin(), m_size);
        }
    }

    /**
     * @brief Construct an array from a memory range.
     * @param begin Pointer to the memory's begin.
     * @param end Pointer to the memory's end.
    */
    constexpr TArray(const ItemType* begin, const ItemType* end)
    {
        AE_ASSERT(begin && end);

        const uint64   beginAddr = reinterpret_cast<uint64>(begin);
        const uint64   endAddr   = reinterpret_cast<uint64>(end);
        const SizeType size      = static_cast<SizeType>(endAddr - beginAddr) / sizeof(ItemType);

        AE_ASSERT(size);

        m_size     = size;
        m_capacity = size;

        m_allocator.Reallocate(m_size, sizeof(ItemType));
        MemoryUtils::CopyElements(GetData(), begin, m_size);
    }

    ~TArray() {}

    /**
     * @brief Copy assingment constructor.
     * @param other Array to be copied.
     * @return Reference to the constructed array.
    */
    constexpr TArray& operator=(const TArray& other)
    {
        if (this != std::addressof(other)) {
            MemoryUtils::DestroyElements(GetData(), m_size);

            m_size     = other.m_size;
            m_capacity = other.m_capacity;

            m_allocator.Reallocate(m_size, sizeof(ItemType));

            MemoryUtils::CopyElements(GetData(), other.GetData(), m_size);
        }

        return *this;
    }

    /**
     * @brief Copy assingment constructor.
     * @param other Array to be copied.
     * @return Reference to the constructed array.
    */
    constexpr TArray& operator=(TArray&& other) noexcept
    {
        if (this != std::addressof(other)) {
            MemoryUtils::DestroyElements(GetData(), m_size);

            m_size     = other.m_size;
            m_capacity = other.m_capacity;

            m_allocator = std::move(other.m_allocator);

            other.m_size     = 0;
            other.m_capacity = 0;
        }
        return *this;
    }

    /**
     * @brief Returns a pointer to the array's first element.
     * @return Pointer to the array's first element.
    */
    constexpr ItemType* GetData() { return reinterpret_cast<ItemType*>(m_allocator.GetData()); }

    /**
     * @brief Returns a pointer to the array's first element.
     * @return Pointer to the array's first element.
    */
    constexpr const ItemType* GetData() const { return reinterpret_cast<const ItemType*>(m_allocator.GetData()); }

    /**
     * @brief C++ iterator begin.
     * @return A pointer to the begin.
    */
    constexpr ItemType* begin() { return GetData(); }

    /**
     * @brief C++ iterator begin.
     * @return A pointer to the begin.
    */
    constexpr const ItemType* begin() const { return GetData(); }

    /**
     * @brief C++ iterator end.
     * @return A pointer to the end.
    */
    constexpr ItemType* end() { return (begin() + m_size); }

    /**
     * @brief C++ iterator end.
     * @return A pointer to the end.
    */
    constexpr const ItemType* end() const { return (begin() + m_size); }

    /**
     * @brief Returns the number of elements present on the array.
     * @return Number of elements of the array.
    */
    constexpr SizeType GetSize() const { return m_size; }

    constexpr size_t GetSizeInBytes() const { return (size_t)GetCapacity() * sizeof(ItemType); }

    /**
     * @brief Returns the capacity of the array.
     * @return The capacity (in bytes) of the array.
    */
    constexpr SizeType GetCapacity() const { return m_capacity; }

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
    constexpr ItemType& operator[](SizeType pos)
    {
        AE_ASSERT(pos < GetSize());
        return GetData()[pos];
    }

    /**
     * @brief Returns an element at a position (index).
     * @param pos The position (index) of the desired element.
     * @return The element at the position pos.
    */
    constexpr const ItemType& operator[](SizeType pos) const
    {
        AE_ASSERT(pos < GetSize());
        return GetData()[pos];
    }

    /**
     * @brief Inserts a new element into the container directly before pos.
     * @param pos Position to insert new element to.
     * @return Index to the inserted element.
    */
    template<class... ArgsType>
    constexpr SizeType Emplace(ArgsType&&... args)
    {
        const SizeType index = AddSlots(1);
        new (GetData() + index) ItemType(std::forward<ArgsType>(args)...);
        return index;
    }

    /**
     * @brief Adds an element to the end of the array.
     * The array will be resized if needed.
     * @param element The element that will be added.
     * @return Reference to the added element.
    */
    constexpr ItemType& Add(ItemType&& element)
    {
        CheckAddress(&element);
        const SizeType index = Emplace(std::move(element));
        return GetData()[index];
    }

    /**
     * @brief Adds an element to the end of the array.
     * The array will be resized if needed.
     * @param element The element that will be added.
     * @return Reference to the added element.
    */
    constexpr ItemType& Add(const ItemType& element)
    {
        CheckAddress(&element);
        const SizeType index = Emplace(element);
        return GetData()[index];
    }

    /**
     * @brief Adds empty slots at the end of the array.
     * @param slots Number of slots to add.
     * @return The old size of the array.
    */
    constexpr SizeType AddSlots(SizeType slots)
    {
        AE_ASSERT(slots > 0);
        const SizeType oldSize = GetSize();
        if ((m_size += slots) > m_capacity) {
            ResizeImpl();
        }

        return oldSize;
    }

    /**
     * @brief Resizes the container to contain count elements.
     * @param count New size.
    */
    constexpr void Reserve(uint32 newCapacity)
    {
        AE_ASSERT(newCapacity >= 0);
        if (newCapacity) {
            newCapacity = m_allocator.CalculateReserve(newCapacity);
        }
        if (newCapacity != m_capacity) {
            m_capacity = newCapacity;
            m_allocator.Reallocate(newCapacity, sizeof(ItemType));
        }
    }

    /**
     * @brief Requests the removal of unused capacity.
    */
    constexpr void ShrinkToFit() {}

    /**
     * @brief Removes a count of elements starting at an index.
     * @param index Starting index of the remotion.
     * @param count Number of elements to remove.
    */
    constexpr void RemoveAt(SizeType index, SizeType count = 1)
    {
        AE_ASSERT(count > 0);
        AE_ASSERT((index + count) < GetSize());
        AE_ASSERT(index < GetSize());

        for (SizeType i = index; i < GetSize(); i++) {
            GetData()[i] = std::move(GetData()[i + count]);
        }
    }

  private:
    /**
     * @brief Verify if the addres is not present in the array.
     * @param address 
    */
    constexpr void CheckAddress(const ItemType* address) const { AE_ASSERT(address < GetData() || address >= (GetData() + GetCapacity())); }

    constexpr void ResizeImpl()
    {
        m_capacity = m_allocator.CalculateGrowth(m_size, m_capacity);
        m_allocator.Reallocate(m_capacity, sizeof(ItemType));
    }

  private:
    AllocatorType m_allocator;
    SizeType      m_size;
    SizeType      m_capacity;
};

template<class ItemType, class AllocatorType>
[[nodiscard]] constexpr bool
operator==(const TArray<ItemType, AllocatorType>& lhs, const TArray<ItemType, AllocatorType>& rhs)
{
    return lhs.GetSize() == rhs.GetSize() && std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<class ItemType, class AllocatorType>
[[nodiscard]] constexpr bool
operator!=(const TArray<ItemType, AllocatorType>& lhs, const TArray<ItemType, AllocatorType>& rhs)
{
    return !(lhs == rhs);
}

template<class ItemType, class AllocatorType>
[[nodiscard]] constexpr bool
operator<(const TArray<ItemType, AllocatorType>& lhs, const TArray<ItemType, AllocatorType>& rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<class ItemType, class AllocatorType>
[[nodiscard]] constexpr bool
operator<=(const TArray<ItemType, AllocatorType>& lhs, const TArray<ItemType, AllocatorType>& rhs)
{
    return !(rhs < lhs);
}

template<class ItemType, class AllocatorType>
[[nodiscard]] constexpr bool
operator>(const TArray<ItemType, AllocatorType>& lhs, const TArray<ItemType, AllocatorType>& rhs)
{
    return rhs < lhs;
}

template<class ItemType, class AllocatorType>
[[nodiscard]] constexpr bool
operator>=(const TArray<ItemType, AllocatorType>& lhs, const TArray<ItemType, AllocatorType>& rhs)
{
    return !(lhs < rhs);
}