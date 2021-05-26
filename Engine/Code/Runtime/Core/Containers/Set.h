#pragma once

#include "Array.h"
#include "Misc/CRC32.h"

class SetKey
{
    template<class T, class THash, class TComp>
    friend class TSet;

  public:
    constexpr SetKey()
      : Index(~0u)
    {}

    constexpr explicit SetKey(uint32 index)
      : Index(index)
    {}

    constexpr bool IsValid() const { return Index != ~0u; }

  private:
    uint32 Index;
};

template<class T, class THash = TCRC32<T>, class TComp = std::less<T>>
class TSet
{
  public:
    using ValueType = T;
    using HashFunc  = THash;
    using CompFunc  = TComp;

    /**
     * @brief Default constructor.
    */
    constexpr explicit TSet() noexcept
      : m_lookup(8)
    {}

    /**
     * @brief Adds an item to the container.
     * @param item Item to be added.
    */
    constexpr void Add(ValueType&& item)
    {
        const uint32 currIndex = m_data.GetSize();
        const uint64 hash      = (uint64)THash()(item);

        if (currIndex >= (m_lookup.GetSize() - 1)) {
            ReHash();
        }

        if (!HasKey(hash)) {
            const uint64 keyIndex = GetKeyIndex(hash);
            m_lookup[keyIndex]    = SetKey(currIndex);
            m_data.Add(std::move(item));
        }
    }

    constexpr ValueType* FindByKey(uint64 key)
    {
        const uint32 keyIndex = GetKeyIndex(key);
        const SetKey foundKey = m_lookup[keyIndex];

        if (foundKey.IsValid()) {
            const uint32 index = foundKey.Index;
            return &m_data[index];
        }

        return nullptr;
    }

    constexpr ValueType* Find(const ValueType& item)
    {
        const uint64 hash = (uint64)THash()(item);
        return FindByKey(hash);
    }

    constexpr const ValueType* Find(const ValueType& item) const
    {
        const uint64 hash = (uint64)THash()(item);
        return FindByKey(hash);
    }

    constexpr bool HasItem(const ValueType& item) { return Find(item) != nullptr; }

    constexpr bool HasKey(uint64 key) { return FindByKey(key) != nullptr; }

    /**
     * @brief Returns the number of itens inside the Set.
     * @return The number of itens inside the Set.
    */
    constexpr uint32 GetCount() const { return 0; }

    /**
     * @brief Check if the set is empty.
     * @return True if empty false otherwise.
    */
    constexpr bool IsEmpty() const { return GetCount() == 0; }

    /**
     * @brief Returns the internal TArray's size.
     * @return The internal TArray's size.
    */
    constexpr uint32 GetSize() const { return m_data.GetSize(); }

    /**
     * @brief Returns the internal TArray's capacity.
     * @return The internal TArray's capacity.
    */
    constexpr uint32 GetCapacity() const { return m_data.GetCapacity(); }

  private:
    constexpr void ReHash()
    {
        const uint32 currSize = m_lookup.GetSize();
        const uint32 newSize  = currSize * 2;
        m_lookup.Resize(newSize);

        TArray<SetKey> tempLookup(newSize);

        for (uint32 i = 0; i < currSize; i++) {
            SetKey& key = m_lookup[i];
            if (key.IsValid()) {
                uint32 index         = key.Index;
                uint32 hash          = THash()(m_data[index]);
                uint32 newIndex      = GetKeyIndex(hash);
                tempLookup[newIndex] = m_lookup[i];
            }
        }

        m_lookup = std::move(tempLookup);
    }

    constexpr uint64 GetKeyIndex(uint64 key) const
    {
        const uint64 index = key & (uint64)(m_lookup.GetSize() - 1u);
        return index;
    }

  private:
    TArray<ValueType> m_data;
    TArray<SetKey>    m_lookup;
};