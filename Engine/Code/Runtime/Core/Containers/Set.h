#pragma once

#include "Array.h"
#include "Misc/CRC32.h"

template<class T, class THash = TCRC32<T>>
class TSet
{
  public:
    using ValueType = T;

    constexpr explicit TSet(uint32 initialSize = 1024)
      : m_data(initialSize)
      , m_usedIndices(initialSize)
      , m_count(0)
    {}

    constexpr void Add(ValueType&& item) { InsertItem<ValueType&&>(std::move(item)); }

    constexpr void Add(const ValueType& item) { InsertItem<const ValueType&>(item); }

    constexpr void Remove(const ValueType& item)
    {
        const uint32 index = GetIndex(item);
        if (m_usedIndices[index]) {
            m_count--;
            m_usedIndices[index] = false;
        }
    }

  private:
    constexpr uint32 GetIndex(const ValueType& item)
    {
        const uint32 hash  = static_cast<uint32>(m_hash(item));
        const uint32 index = hash & (m_data.GetSize() - 1);
        return index;
    }

    template<class TItem>
    constexpr void InsertItem(typename TIdentity<TItem>::Type item)
    {
        const uint32 index = GetIndex(item);
        if (!m_usedIndices[index]) {
            m_data[index]        = std::move(item);
            m_usedIndices[index] = true;
            m_count++;
        }
        
    }

  private:
    TArray<T>    m_data;
    TArray<bool> m_usedIndices;
    THash        m_hash;
    uint32       m_count;
};