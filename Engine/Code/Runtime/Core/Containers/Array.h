#pragma once

#include "ContainerAllocators.h"
#include "Serialization/Archive.h"

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
        m_capacity = m_size;

        m_allocator.Reallocate(m_size, 0, sizeof(ItemType));
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
        m_capacity = m_size;

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

        m_allocator.Reallocate(m_size, 0, sizeof(ItemType));

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

            m_allocator.Reallocate(m_size, 0, sizeof(ItemType));
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

        m_allocator.Reallocate(m_size, 0, sizeof(ItemType));
        MemoryUtils::CopyElements(GetData(), begin, m_size);
    }

    constexpr TArray(const TArray& other, SizeType extraSpace)
    {
        m_size     = other.m_size + extraSpace;
        m_capacity = m_size;

        m_allocator.Reallocate(m_size, 0, sizeof(ItemType));
        MemoryUtils::CopyElements(GetData(), other.GetData(), other.GetSize());
        MemoryUtils::ConstructElements(GetData() + other.GetSize(), extraSpace);
    }

    constexpr TArray(TArray&& other, SizeType extraSpace)
    {
        m_size     = other.m_size + extraSpace;
        m_capacity = m_size;

        m_allocator = std::move(other.m_allocator);
        Reserve(m_size);

        other.m_size     = 0;
        other.m_capacity = 0;
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
            MemoryUtils::DestroyItems(GetData(), m_size);

            const SizeType oldSize = m_size;

            m_size     = other.m_size;
            m_capacity = other.m_capacity;

            m_allocator.Reallocate(m_size, oldSize, sizeof(ItemType));

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
            MemoryUtils::DestroyItems(GetData(), m_size);

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

    constexpr size_t GetSizeInBytes() const
    {
        return ((size_t)GetCapacity() * sizeof(ItemType)) + sizeof(m_size) + sizeof(m_capacity) + sizeof(m_allocator);
    }

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
     * @brief Inserts an item at given index.
     * Inserts using Copy semantics.
     * @param index Index where to insert the item.
     * @param item Item to be inserted.
     * @return Index at where the item was inserted.
    */
    SizeType Insert(SizeType index, const ItemType& item)
    {
        CheckAddress(&item);

        InsertImpl(index, 1);
        new (GetData() + index) ItemType(item);
        return index;
    }

    /**
     * @brief Inserts an item at given index.
     * Inserts using Move semantics.
     * @param index Index where to insert the item.
     * @param item Item to be inserted.
     * @return Index at where the item was inserted.
    */
    SizeType Insert(SizeType index, ItemType&& item)
    {
        CheckAddress(&item);

        InsertImpl(index, 1);
        new (GetData() + index) ItemType(std::move(item));
        return index;
    }

    /**
     * @brief Inserts a list at given index.
     * Inserts using Move semantics.
     * @param index Index where to insert the item.
     * @param list List to be inserted.
     * @return Index at where the list was inserted.
    */
    SizeType Insert(const SizeType index, std::initializer_list<ItemType> list)
    {
        SizeType newCount = static_cast<SizeType>(list.size());

        InsertImpl(index, newCount);
        MemoryUtils::CopyElements(GetData() + index, list.begin(), newCount);

        return index;
    }

    /**
     * @brief Resizes the container to contain count elements.
     * @param count New size.
    */
    constexpr void Reserve(SizeType newCapacity)
    {
        AE_ASSERT(newCapacity >= 0);
        if (newCapacity) {
            newCapacity = m_allocator.CalculateReserve(newCapacity);
        }
        if (newCapacity != m_capacity) {
            const SizeType oldCapacity = m_capacity;
            m_capacity                 = newCapacity;
            m_allocator.Reallocate(newCapacity, oldCapacity, sizeof(ItemType));
        }
    }

    /**
     * @brief Resizes the array to a new size.
     * @param newSize New size.
    */
    constexpr void Resize(SizeType newSize)
    {
        if (newSize < m_size) {
            const SizeType index = m_size - newSize;
            const SizeType count = index;
            RemoveAt(index, count, false);
        } else if (newSize > m_size) {
            m_size = newSize;
            ResizeImpl();
        }
    }

    constexpr ItemType Pop(bool shrink = true)
    {
        AE_ASSERT(!IsEmpty());
        ItemType result = std::move(GetData()[m_size - 1]);
        RemoveAt(m_size - 1, 1, shrink);
        return result;
    }

    /**
     * @brief Clears array and destroy its items.
    */
    constexpr void Clear(bool shrink = false)
    {
        MemoryUtils::DestroyItems(GetData(), m_size);
        m_size = 0;
        if (shrink) {
            ShrinkToFit();
        }
    }

    /**
     * @brief Checks if item is contained by the array.
     * @param item Item to be looked for.
     * @return True if item is inside array, else otherwise.
    */
    template<class ComparisonType>
    bool Contains(const ComparisonType& item) const
    {
        for (const ItemType* __restrict data = GetData(), *__restrict dataEnd = data + m_size; data != dataEnd; ++data) {
            if (*data == item) {
                return true;
            }
        }
        return false;
    }

    SizeType Find(const ItemType& item) const
    {
        const ItemType* __restrict start = GetData();
        for (const ItemType* __restrict data = start, *__restrict dataEnd = data + m_size; data != dataEnd; ++data) {
            if (*data == item) {
                return static_cast<SizeType>(data - start);
            }
        }
        return INVALID_INDEX;
    }

    /**
     * @brief Requests the removal of unused capacity.
    */
    constexpr void ShrinkToFit()
    {
        if (m_size != m_capacity) {
            Reserve(m_size);
        }
    }

    /**
     * @brief Removes a count of elements starting at an index.
     * @param index Starting index of the remotion.
     * @param count Number of elements to remove.
    */
    constexpr void RemoveAt(SizeType index, SizeType count = 1, bool shrink = true)
    {
        RemoveAtImpl(index, count);
        if (shrink) {
            ShrinkToFit();
        }
    }

    friend void Serialize(Archive& ar, TArray& arr, const char* name, const char* label)
    {
        SizeType arraySize = 0;
        if (ar.IsWriting()) {
            arraySize = arr.m_size;
        }

        ar(arraySize, "size", "Size");

        if (arraySize == 0) {
            if (ar.IsReading()) {
                arr.Clear(true);
            }
            return;
        }

        if (sizeof(ItemType) == 0 || TIsFundamental<ItemType>::Value) {
            if ((arr.m_size || arr.m_capacity) && ar.IsReading()) {
                arr.Resize(arr.m_size);
            }

            ar(Archive::RangedBytes(arr.GetData(), arr.GetSize()), "data", "Data");
        } else if (ar.IsReading()) {
            arr.Clear(true);

            for (SizeType i = 0; i < arraySize; i++) {
                ar(*::new (arr) ItemType, "", "");
            }
        } else {
            arr.m_size = arraySize;

            for (SizeType i = 0; i < arr.m_size; i++) {
                ar(arr[i], "", "");
            }
        }

        AE_ASSERT(arraySize >= 0);
    }

  private:
    /**
     * @brief Verify if the addres is not present in the array.
     * @param address 
    */
    constexpr void CheckAddress(const ItemType* address) const { AE_ASSERT(address < GetData() || address >= (GetData() + GetCapacity())); }

    constexpr void ResizeImpl()
    {
        const SizeType oldCapacity = m_capacity;
        m_capacity                 = m_allocator.CalculateGrowth(m_size, m_capacity);
        m_allocator.Reallocate(m_capacity, oldCapacity, sizeof(ItemType));
    }

    constexpr void RemoveAtImpl(SizeType index, SizeType count)
    {
        if (count) {
            AE_ASSERT((count >= 0) & (index >= 0) & (index + count <= m_size));

            MemoryUtils::DestroyItems(GetData() + index, count);

            SizeType moveCount = m_size - index - count;
            if (moveCount) {
                memmove((uint8*)m_allocator.GetData() + (index) * sizeof(ItemType),
                        (uint8*)m_allocator.GetData() + (index + count) * sizeof(ItemType),
                        moveCount * sizeof(ItemType));
            }
            m_size -= count;
        }
    }

    template<class OtherSizeType>
    void InsertImpl(SizeType index, OtherSizeType count)
    {
        AE_ASSERT((count >= 0) & (index >= 0) & (index <= m_size));

        SizeType newCount = count;
        AE_ASSERT(static_cast<OtherSizeType>(newCount) == count);

        const SizeType oldCount = m_size;
        if ((m_size += count) > m_capacity) {
            ResizeImpl();
        }
        ItemType* data = GetData() + index;
        MemoryUtils::MemoryMove<ItemType, ItemType, SizeType>(data + count, data, oldCount - index);
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

template<typename T, typename AllocatorType>
void*
operator new(size_t size, TArray<T, AllocatorType>& arr)
{
    AE_ASSERT(size == sizeof(T));
    const auto index = arr.AddSlots(1);
    return &arr[index];
}

template<class _AllocType>
class TArray<bool, _AllocType>
{
  public:
    using ItemType       = uint8;
    using TargetItemType = bool;
    using AllocatorType  = _AllocType;
    using SizeType       = typename _AllocType::SizeType;

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

        m_allocator.Reallocate(other.m_size, 0, sizeof(ItemType));
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

        m_allocator.Reallocate(m_size, 0, sizeof(ItemType));

        MemoryUtils::ConstructElements(GetData(), m_size);
    }

    /**
     * @brief Initializer List constructor.
     * @param list List to be copied to the new created array.
    */
    constexpr TArray(std::initializer_list<TargetItemType> list)
    {
        const SizeType size = static_cast<SizeType>(list.size());
        if (size) {
            m_size     = size;
            m_capacity = size;

            m_allocator.Reallocate(m_size, 0, sizeof(ItemType));
            MemoryUtils::CopyElements(GetData(), list.begin(), m_size);
        }
    }

    /**
     * @brief Construct an array from a memory range.
     * @param begin Pointer to the memory's begin.
     * @param end Pointer to the memory's end.
    */
    constexpr TArray(const TargetItemType* begin, const TargetItemType* end)
    {
        AE_ASSERT(begin && end);

        const uint64 beginAddr = reinterpret_cast<uint64>(begin);
        const uint64 endAddr   = reinterpret_cast<uint64>(end);
        SizeType     size      = static_cast<SizeType>(endAddr - beginAddr) / sizeof(ItemType);

        AE_ASSERT(size);

        ConvertBoolToBytes(nullptr, begin, &m_size, size);

        m_capacity = m_size;

        m_allocator.Reallocate(m_size, 0, sizeof(ItemType));
        ConvertBoolToBytes(GetData(), begin, &m_size, size);
    }

    ~TArray() {}

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
     * @brief Returns a pointer to the array's first element.
     * @return Pointer to the array's first element.
    */
    constexpr ItemType* GetData() { return reinterpret_cast<ItemType*>(m_allocator.GetData()); }

    /**
     * @brief Returns a pointer to the array's first element.
     * @return Pointer to the array's first element.
    */
    constexpr const ItemType* GetData() const { return reinterpret_cast<const ItemType*>(m_allocator.GetData()); }

  private:
    void ConvertBoolToBytes(uint8* dst, const bool* src, SizeType* dstSize, SizeType srvSize)
    {
        AE_ASSERT(src);

        *dstSize = (srvSize / 8 + 1) * 8;

        if (dst) {}
    }

  private:
    AllocatorType m_allocator;
    SizeType      m_size;
    SizeType      m_capacity;
};