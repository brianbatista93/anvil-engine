#pragma once

#include "Array.h"
#include "Math/AnvilMath.h"
#include "Misc/CRC32.h"
#include "Misc/StdHash.h"

template<class T>
constexpr void
MoveByRealocate(T& a, T& b)
{
    a.~T();

    T*       dest   = &a;
    const T* source = &b;

    typedef T DestructbleT;

    new (dest) T(*source);
    ++(T*&)dest;
    (source++)->DestructbleT::~DestructbleT();
}

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

template<class TItem>
class TSetItem
{
  public:
    constexpr TSetItem() {}

    /** Initialization constructor. */
    template<typename InitType>
    explicit constexpr TSetItem(InitType&& InValue)
      : Value(std::forward<InitType>(InValue))
    {}

    TSetItem(TSetItem&&)      = default;
    TSetItem(const TSetItem&) = default;
    TSetItem& operator=(TSetItem&&) = default;
    TSetItem& operator=(const TSetItem&) = default;

    mutable SetIndexId NextIndexId;
    mutable int32      Index = 0;
    TItem              Value;
};

template<class T, class THash = TCRC32<T>, class TComp = std::equal_to<THash::ResultType>>
class TSet
{
  public:
    using ItemType  = TSetItem<T>;
    using HashFunc   = THash;
    using CompFunc   = TComp;
    using HashResult = typename THash::ResultType;

    /**
     * @brief Default constructor.
    */
    constexpr explicit TSet() noexcept
      : m_hashSize(0)
    {}

    constexpr TSet(const TSet& other)
      : m_hashSize(0)
    {
        *this = other;
    }

    constexpr explicit TSet(const TArray<T>& arr)
      : m_hashSize(0)
    {
        Append(arr);
    }

    constexpr explicit TSet(TArray<T>&& arr)
      : m_hashSize(0)
    {
        Append(std::move(arr));
    }

    constexpr TSet(std::initializer_list<T> list)
      : m_hashSize(0)
    {
        Append(list);
    }

    /**
     * @brief Adds an item to the container.
     * @param item Item to be added.
    */
    constexpr void Add(T&& item) { Emplace(std::move(item)); }

    /**
     * @brief Adds an item to the container.
     * @param item Item to be added.
    */
    constexpr void Add(const T& item) { Emplace(item); }

    template<class Args>
    constexpr SetIndexId Emplace(Args&& args)
    {
        const uint32 bucketIndex   = m_items.AddSlots(1);
        void*        bucketAddress = reinterpret_cast<void*>(m_items.GetData() + bucketIndex);
        ItemType&   item          = *new (bucketAddress) ItemType(std::forward<Args>(args));
        SetIndexId   indexId(bucketIndex);

        HashResult hash = static_cast<HashResult>(THash()(item.Value));
        if (!ReplaceExisting(hash, item, indexId)) {
            RehashOrLink(hash, item, indexId);
        }

        return indexId;
    }

    constexpr void RehashOrLink(HashResult hash, ItemType& item, SetIndexId itemId)
    {
        if (!ConditionalRehash(m_items.GetSize())) {
            LinkItem(itemId, item, hash);
        }
    }

    constexpr SetIndexId FindId(const T& key) const
    {
        if (!m_items.IsEmpty()) {
            const HashResult hash = THash()(key);
            for (SetIndexId itemId = GetTypedHash(hash); itemId.IsValid(); itemId = m_items[itemId].NextIndexId) {
                const HashResult otherHash = THash()(m_items[itemId].Value);
                if (TComp()(hash, otherHash)) {
                    return itemId;
                }
            }
        }

        return SetIndexId();
    }

    constexpr T* Find(const T& key)
    {
        SetIndexId itemId = FindId(key);
        if (itemId.IsValid()) {
            return &m_items[itemId].Value;
        }
        return nullptr;
    }

    constexpr const T* Find(const T& key) const { return const_cast<TSet*>(this)->Find(key); }

    constexpr SetIndexId FindIdByHash(HashResult hash) const
    {
        if (!m_items.IsEmpty()) {
            for (SetIndexId itemId = GetTypedHash(hash); itemId.IsValid(); itemId = m_items[itemId].NextIndexId) {
                HashResult otherHash = static_cast<HashResult>(THash()(m_items[itemId].Value));
                if (TComp()(hash, otherHash)) {
                    return itemId;
                }
            }
        }

        return SetIndexId();
    }

    constexpr bool ConditionalRehash(uint32 hashCount)
    {
        const uint32 desiredHashSize = (hashCount >= 4) ? Math::RoundUpToPowerOfTwo(hashCount / 2 + 8) : 1;

        if (ShouldRehash(hashCount, desiredHashSize)) {
            m_hashSize = desiredHashSize;
            Rehash();
            return true;
        }

        return false;
    }

    constexpr bool Contains(const T& key)
    {
        const HashResult hash = THash()(key);
        return FindIdByHash(hash).IsValid();
    }

    constexpr void Rehash()
    {
        m_hashes.Resize(m_hashSize);

        for (uint32 i = 0; i < m_hashSize; i++) {
            GetTypedHash(i) = SetIndexId();
        }

        for (auto& it : m_items) {
            HashItem(SetIndexId(it.Index), it);
        }
    }

    constexpr bool ShouldRehash(uint32 NumHashedElements, uint32 DesiredHashSize) const
    {
        return (!m_hashSize || m_hashSize < DesiredHashSize);
    }

    constexpr void HashItem(SetIndexId itemId, const ItemType& item) const { LinkItem(itemId, item, THash()(item.Value)); }

    constexpr void LinkItem(SetIndexId itemId, const ItemType& item, HashResult hash) const
    {
        const HashResult index = hash & (m_hashSize - 1);
        item.Index             = static_cast<uint32>(index);

        item.NextIndexId         = GetTypedHash(item.Index);
        GetTypedHash(item.Index) = itemId;
    }

    constexpr bool ReplaceExisting(HashResult hash, ItemType& item, SetIndexId& id)
    {
        bool isValid = false;
        if (m_items.GetSize() != 1) {
            SetIndexId id = FindIdByHash(hash);
            isValid       = id.IsValid();
            if (isValid) {
                MoveByRealocate(m_items[id].Value, item.Value);

                return true;
            }
        }

        return false;
    }

    constexpr void Append(const TArray<T>& arr)
    {
        Reserve(m_items.GetSize() + arr.GetSize());
        for (const T& item : arr) {
            Add(item);
        }
    }

    constexpr void Append(TArray<T>&& arr)
    {
        Reserve(m_items.GetSize() + arr.GetSize());
        for (T& item : arr) {
            Add(std::move(item));
        }

        arr.Clear();
    }

    constexpr void Append(std::initializer_list<T> list)
    {
        Reserve(m_items.GetSize() + (uint32)list.size());
        for (const T& item : list) {
            Add(item);
        }
    }

    constexpr T& operator[](SetIndexId id) { return m_items[id].Value; }

    constexpr const T& operator[](SetIndexId id) const { return m_items[id].Value; }

    /**
     * @brief Returns the number of itens inside the Set.
     * @return The number of itens inside the Set.
    */
    constexpr uint32 GetCount() const { return m_items.GetSize(); }

    constexpr size_t GetSizeInBytes() const { return m_items.GetSizeInBytes() + m_hashes.GetSizeInBytes() + sizeof(m_hashSize); }

    /**
     * @brief Check if the set is empty.
     * @return True if empty false otherwise.
    */
    constexpr bool IsEmpty() const { return GetCount() == 0; }

    /**
     * @brief Returns the internal TArray's capacity.
     * @return The internal TArray's capacity.
    */
    constexpr uint32 GetCapacity() const { return m_items.GetCapacity(); }

    constexpr SetIndexId& GetTypedHash(HashResult hashIndex) const
    {
        const uint32 hashSize = m_hashes.GetSize();
        return ((SetIndexId*)m_hashes.GetData())[hashIndex & (hashSize - 1)];
    }

  private:
    TArray<ItemType>  m_items;
    TArray<SetIndexId> m_hashes;
    uint32             m_hashSize = 0;
};
