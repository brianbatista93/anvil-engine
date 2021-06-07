#pragma once

#include "Set.h"

template<class TKey, class TValue>
struct TPair
{
    TKey   Key;
    TValue Value;

    constexpr TPair() {}

    constexpr TPair(TKey key, TValue value)
      : Key(key)
      , Value(value)
    {}

    template<class TOtherKey, class TOtherValue>
    constexpr TPair(const TPair<TOtherKey, TOtherValue>& other)
      : Key(other.Key)
      , Value(other.Value)
    {}
};

template<typename K, typename V>
struct TCRC32<TPair<K, V>>
{
    using ResultType = uint32;
    ResultType operator()(const TPair<K, V>& pair) const { return TCRC32<K>()(pair.Key); }
};

template<class TKey, class TValue, class THash = TCRC32<TPair<TKey, TValue>>, class TComp = std::equal_to<THash::ResultType>>
class TMap
{
  public:
    using KeyType   = TKey;
    using ItemType = TValue;
    using PairType  = TPair<KeyType, ItemType>;

    constexpr TMap() {}

    /**
     * @brief Adds an item to the container.
     * @param item Item to be added.
    */
    constexpr ItemType& Add(const KeyType& key, const ItemType& item) { return Emplace(key, item); }
    constexpr ItemType& Add(const KeyType& key, ItemType&& item) { return Emplace(key, std::move(item)); }
    constexpr ItemType& Add(KeyType&& key, const ItemType& item) { return Emplace(std::move(key), item); }
    constexpr ItemType& Add(KeyType&& key, ItemType&& item) { return Emplace(std::move(key), std::move(item)); }

    template<class TTKey, class TTValue>
    ItemType& Emplace(TTKey&& key, TTValue&& value)
    {
        const SetIndexId pairId = m_pairs.Emplace(TPair<TTKey, TTValue>(std::forward<TTKey>(key), std::forward<TTValue>(value)));
        return m_pairs[pairId].Value;
    }

    /**
     * @brief Returns the number of itens inside the Set.
     * @return The number of itens inside the Set.
    */
    constexpr uint32 GetCount() const { return m_pairs.GetCount(); }

    constexpr size_t GetSizeInBytes() const { return m_pairs.GetSizeInBytes(); }

    /**
     * @brief Check if the set is empty.
     * @return True if empty false otherwise.
    */
    constexpr bool IsEmpty() const { return GetCount() == 0; }

    /**
     * @brief Returns the internal TArray's capacity.
     * @return The internal TArray's capacity.
    */
    constexpr uint32 GetCapacity() const { return m_pairs.GetCapacity(); }

  private:
    TSet<PairType, THash, TComp> m_pairs;
};