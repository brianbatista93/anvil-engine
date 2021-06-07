#pragma once

#include "Memory/Allocators/DynamicAllocator.h"
#include "Misc/CRC32.h"

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
    constexpr explicit TArray() noexcept(noexcept(AllocatorType())) {}

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
    constexpr TArray(TArray&& other) { m_allocator = std::move(other.m_allocator); }

    /**
     * @brief Constructs a array with an initial slots count.
     * The slots are default initialized elements.
     * @param count The initial slots count.
    */
    constexpr TArray(SizeType count)
    {
        m_allocator.Allocate(count);
        MemoryUtils::ConstructElements(GetData(), GetSize());
    }

    /**
     * @brief Initializer List constructor.
     * @param list List to be copied to the new created array.
    */
    constexpr TArray(std::initializer_list<ItemType> list)
    {
        if (list.size()) {
            m_allocator.Allocate(list.size());
            MemoryUtils::CopyElements(GetData(), list.begin(), list.size());
        }
    }

    /**
     * @brief Construct an array from a memory range.
     * @param begin Pointer to the memory's begin.
     * @param end Pointer to the memory's end.
    */
    constexpr TArray(const ItemType* begin, const ItemType* end)
    {
        const uint64 beginAddr = reinterpret_cast<uint64>(begin);
        const uint64 endAddr   = reinterpret_cast<uint64>(end);
        const SizeType size      = static_cast<SizeType>(endAddr - beginAddr);

        AE_ASSERT(size);

        m_allocator.Allocate(size / sizeof(ItemType));
        MemoryUtils::CopyMemory(GetData(), begin, size);
    }

    ~TArray() noexcept { m_allocator.Destroy(); }

    /**
     * @brief Copy assingment constructor.
     * @param other Array to be copied.
     * @return Reference to the constructed array.
    */
    constexpr TArray& operator=(const TArray& other)
    {
        if (!IsEmpty()) {
            m_allocator.Destroy();
        }

        if (!other.IsEmpty()) {

            const SizeType otherSize = other.GetSize();

            m_allocator.Allocate(otherSize);

            MemoryUtils::CopyElements(GetData(), other.GetData(), otherSize);
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
        if (this == std::addressof(other)) {
            return *this;
        }

        if (!IsEmpty()) {
            m_allocator.Destroy();
        }

        if (!other.IsEmpty()) {
            m_allocator = std::move(other.m_allocator);
        }

        return *this;
    }

    /**
     * @brief Returns a pointer to the array's first element.
     * @return Pointer to the array's first element.
    */
    constexpr ItemType* GetData() { return m_allocator.GetData(); }

    /**
     * @brief Returns a pointer to the array's first element.
     * @return Pointer to the array's first element.
    */
    constexpr const ItemType* GetData() const { return m_allocator.GetData(); }

    /**
     * @brief C++ iterator begin.
     * @return A pointer to the begin.
    */
    constexpr ItemType* begin() noexcept { return m_allocator.begin(); }

    /**
     * @brief C++ iterator begin.
     * @return A pointer to the begin.
    */
    constexpr const ItemType* begin() const noexcept { return m_allocator.begin(); }

    /**
     * @brief C++ iterator end.
     * @return A pointer to the end.
    */
    constexpr ItemType* end() noexcept { return m_allocator.end(); }

    /**
     * @brief C++ iterator end.
     * @return A pointer to the end.
    */
    constexpr const ItemType* end() const noexcept { return m_allocator.end(); }

    /**
     * @brief Returns the number of elements present on the array.
     * @return Number of elements of the array.
    */
    constexpr SizeType GetSize() const { return (SizeType)m_allocator.GetSize(); }

    constexpr size_t GetSizeInBytes() const { return (size_t)GetCapacity() * sizeof(ItemType); }

    /**
     * @brief Returns the capacity of the array.
     * @return The capacity (in bytes) of the array.
    */
    constexpr SizeType GetCapacity() const { return (SizeType)m_allocator.GetCapacity(); }

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
    template<class... Args>
    constexpr SizeType Emplace(SizeType pos, Args&&... args)
    {
        const SizeType currentSize = GetSize();
        if (currentSize + 1 > GetCapacity()) {
            const SizeType newCapacity = CalculateGrowth(currentSize + 1);
            m_allocator.Reserve(newCapacity);
        }

        if (pos > 0) {
            for (SizeType i = GetSize(); i != pos; i--) {
                std::swap(GetData()[i - 1], GetData()[i]);
            }
        }

        m_allocator.Allocate(1);

        ItemType* ptr = GetData() + pos;

        MemoryUtils::ConstructElements(ptr, 1, std::forward<Args>(args)...);

        return pos;
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
        const SizeType index = Emplace(GetSize(), std::move(element));
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
        const SizeType index = Emplace(GetSize(), element);
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

        Resize(oldSize + slots);

        return oldSize;
    }

    /**
     * @brief Resizes the container to contain count elements.
     * @param count New size.
    */
    template<class... Args>
    constexpr void Resize(SizeType count, Args&&... args)
    {
        const SizeType currentSize = GetSize();
        if (count < currentSize) {
            m_allocator.Reset(count);
        } else {
            m_allocator.Reserve(count);
            MemoryUtils::ConstructElements(GetData() + currentSize, count - currentSize, std::forward<Args>(args)...);
            m_allocator.Allocate(count - currentSize);
        }
    }

    /**
     * @brief Requests the removal of unused capacity.
    */
    constexpr void ShrinkToFit()
    {
        if (GetCapacity() > GetSize()) {
            m_allocator.Reserve(GetSize());
        }
    }

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

        m_allocator.Pop(count);
    }

  private:
    /**
     * @brief Get the new capacity value from a size.
     * @param newSize The size to calculate the new capacity from.
     * @return The new capacity value.
    */
    SizeType CalculateGrowth(SizeType newSize) const
    {
        const SizeType oldCapacity = GetCapacity();
        const SizeType newCapacity = oldCapacity + oldCapacity / 2;

        if (newCapacity < newSize) {
            return newSize;
        }

        return newCapacity;
    }

    /**
     * @brief Verify if the addres is not present in the array.
     * @param address 
    */
    constexpr void CheckAddress(const ItemType* address) const { AE_ASSERT(address < GetData() || address >= (GetData() + GetCapacity())); }

  private:
    AllocatorType m_allocator;
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