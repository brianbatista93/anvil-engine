#pragma once

#include "Array.h"
#include "Misc/CRC32.h"

class SetIndexId
{
    template<class, class, class>
    friend class TSet;

  public:
    enum
    {
        INVALID_INDEX = -1
    };

    constexpr explicit SetIndexId()
      : m_index(INVALID_INDEX)
    {}

    constexpr friend bool operator==(const SetIndexId& a, const SetIndexId& b) { return a.m_index == b.m_index; }

    constexpr bool IsValid() const { return m_index != INVALID_INDEX; }

  private:
    int32 m_index;

    constexpr SetIndexId(int32 index)
      : m_index(index)
    {}

    constexpr operator int32() const { return m_index; }
};

template<class T>
class TSetItem
{
  public:
    constexpr TSetItem() {}

    /** Initialization constructor. */
    template<typename InitType>
    explicit constexpr TSetItem(InitType&& InValue)
      : Data(std::forward<InitType>(InValue))
    {}

    TSetItem(TSetItem&&)      = default;
    TSetItem(const TSetItem&) = default;
    TSetItem& operator=(TSetItem&&) = default;
    TSetItem& operator=(const TSetItem&) = default;

    SetIndexId NextIndexId;
    int32      Index;
    T          Data;
};

template<class T, class THash = TCRC32<T>, class TComp = std::less<T>>
class TSet
{
  public:
    using ValueType = TSetItem<T>;
    using HashFunc  = THash;
    using CompFunc  = TComp;

    /**
     * @brief Default constructor.
    */
    constexpr explicit TSet() noexcept {}

    /**
     * @brief Adds an item to the container.
     * @param item Item to be added.
    */
    constexpr void Add(T&& item) { Emplace(std::move(item)); }

    template<class Args>
    constexpr SetIndexId Emplace(Args&& args)
    {
        const uint32 bucketIndex   = m_data.AddSlots(1);
        void*        bucketAddress = reinterpret_cast<void*>(m_data.GetData() + bucketIndex);
        ValueType&   item          = *new (bucketAddress) ValueType(std::forward<Args>(args));
        SetIndexId   indexId(bucketIndex);

        uint32 hash = static_cast<uint32>(THash()(item.Data));

        // if hash isn't on bucket
        //      if isn't allowed to override
        //          Rehash and Link

        return indexId;
    }

    /**
     * @brief Returns the number of itens inside the Set.
     * @return The number of itens inside the Set.
    */
    constexpr uint32 GetCount() const { return m_data.GetSize(); }

    /**
     * @brief Check if the set is empty.
     * @return True if empty false otherwise.
    */
    constexpr bool IsEmpty() const { return GetCount() == 0; }

    /**
     * @brief Returns the internal TArray's capacity.
     * @return The internal TArray's capacity.
    */
    constexpr uint32 GetCapacity() const { return m_data.GetCapacity(); }

  private:
    TArray<ValueType> m_data;
};