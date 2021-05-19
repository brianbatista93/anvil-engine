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
    constexpr TArray(TArray&& other) { m_allocator = std::move(other.m_allocator); }

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

    /**
     * @brief Initializer List constructor.
     * @param list List to be copied to the new created array.
    */
    constexpr TArray(std::initializer_list<T> list)
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
    constexpr TArray(const T* begin, const T* end)
    {
        const uint64 beginAddr = reinterpret_cast<uint64>(begin);
        const uint64 endAddr   = reinterpret_cast<uint64>(end);
        const uint32 size      = static_cast<uint32>(endAddr - beginAddr);

        AE_ASSERT(size);

        m_allocator.Allocate(size / sizeof(T));
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

            const uint32 otherSize = other.GetSize();

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
    constexpr T* GetData() { return m_allocator.GetData(); }

    /**
     * @brief Returns a pointer to the array's first element.
     * @return Pointer to the array's first element.
    */
    constexpr const T* GetData() const { return m_allocator.GetData(); }

    /**
     * @brief C++ iterator begin.
     * @return A pointer to the begin.
    */
    constexpr T* begin() noexcept { return m_allocator.begin(); }

    /**
     * @brief C++ iterator begin.
     * @return A pointer to the begin.
    */
    constexpr const T* begin() const noexcept { return m_allocator.begin(); }

    /**
     * @brief C++ iterator end.
     * @return A pointer to the end.
    */
    constexpr T* end() noexcept { return m_allocator.end(); }

    /**
     * @brief C++ iterator end.
     * @return A pointer to the end.
    */
    constexpr const T* end() const noexcept { return m_allocator.end(); }

    /**
     * @brief Returns the number of elements present on the array.
     * @return Number of elements of the array.
    */
    constexpr uint32 GetSize() const { return (uint32)m_allocator.GetSize(); }

    /**
     * @brief Returns the capacity of the array.
     * @return The capacity (in bytes) of the array.
    */
    constexpr uint32 GetCapacity() const { return (uint32)m_allocator.GetCapacity(); }

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
        AE_ASSERT(pos < GetSize());
        return GetData()[pos];
    }

    /**
     * @brief Returns an element at a position (index).
     * @param pos The position (index) of the desired element.
     * @return The element at the position pos.
    */
    constexpr const T& operator[](uint32 pos) const
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
    constexpr uint32 Emplace(uint32 pos, Args&&... args)
    {
        const uint32 currentSize = GetSize();
        if (currentSize + 1 > GetCapacity()) {
            const uint32 newCapacity = CalculateGrowth(currentSize + 1);
            m_allocator.Reserve(newCapacity);
        }

        if (pos > 0) {
            for (uint32 i = GetSize(); i != pos; i--) {
                std::swap(GetData()[i - 1], GetData()[i]);
            }
        }

        m_allocator.Allocate(1);

        T* ptr = GetData() + pos;

        MemoryUtils::ConstructElements(ptr, 1, std::forward<Args>(args)...);

        return pos;
    }

    /**
     * @brief Adds an element to the end of the array.
     * The array will be resized if needed.
     * @param element The element that will be added.
     * @return Reference to the added element.
    */
    constexpr T& Add(T&& element)
    {
        CheckAddress(&element);
        const uint32 index = Emplace(GetSize(), std::move(element));
        return GetData()[index];
    }

    /**
     * @brief Adds an element to the end of the array.
     * The array will be resized if needed.
     * @param element The element that will be added.
     * @return Reference to the added element.
    */
    constexpr T& Add(const T& element)
    {
        CheckAddress(&element);
        const uint32 index = Emplace(GetSize(), element);
        return GetData()[index];
    }

    /**
     * @brief Adds empty slots at the end of the array.
     * @param slots Number of slots to add.
     * @return The old size of the array.
    */
    constexpr uint32 AddSlots(uint32 slots)
    {
        AE_ASSERT(slots > 0);
        const uint32 oldSize = GetSize();

        Resize(oldSize + slots);

        return oldSize;
    }

    /**
     * @brief Resizes the container to contain count elements.
     * @param count New size.
    */
    template<class... Args>
    constexpr void Resize(uint32 count, Args&&... args)
    {
        const uint32 currentSize = GetSize();
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
    constexpr void RemoveAt(uint32 index, uint32 count = 1)
    {
        AE_ASSERT(count > 0);
        AE_ASSERT((index + count) < GetSize());
        AE_ASSERT(index < GetSize());

        for (uint32 i = index; i < GetSize(); i++) {
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
    uint32 CalculateGrowth(uint32 newSize) const
    {
        const uint32 oldCapacity = GetCapacity();
        const uint32 newCapacity = oldCapacity + oldCapacity / 2;

        if (newCapacity < newSize) {
            return newSize;
        }

        return newCapacity;
    }

    /**
     * @brief Verify if the addres is not present in the array.
     * @param address 
    */
    constexpr void CheckAddress(const T* address) const { AE_ASSERT(address < GetData() || address >= (GetData() + GetCapacity())); }

  private:
    Alloc m_allocator;
};

template<class T, class Alloc>
[[nodiscard]] constexpr bool
operator==(const TArray<T, Alloc>& lhs, const TArray<T, Alloc>& rhs)
{
    return lhs.GetSize() == rhs.GetSize() && std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<class T, class Alloc>
[[nodiscard]] constexpr bool
operator!=(const TArray<T, Alloc>& lhs, const TArray<T, Alloc>& rhs)
{
    return !(lhs == rhs);
}

template<class T, class Alloc>
[[nodiscard]] constexpr bool
operator<(const TArray<T, Alloc>& lhs, const TArray<T, Alloc>& rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<class T, class Alloc>
[[nodiscard]] constexpr bool
operator<=(const TArray<T, Alloc>& lhs, const TArray<T, Alloc>& rhs)
{
    return !(rhs < lhs);
}

template<class T, class Alloc>
[[nodiscard]] constexpr bool
operator>(const TArray<T, Alloc>& lhs, const TArray<T, Alloc>& rhs)
{
    return rhs < lhs;
}

template<class T, class Alloc>
[[nodiscard]] constexpr bool
operator>=(const TArray<T, Alloc>& lhs, const TArray<T, Alloc>& rhs)
{
    return !(lhs < rhs);
}