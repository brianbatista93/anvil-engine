#pragma once

#include "TypeTraits.h"
#include "Types.h"

struct RangedArchiveBlob
{
    const void* Pointer = nullptr;
    size_t      Size    = 0;

    constexpr bool IsValid() const { return Pointer && Size; }
};

class Archive
{
  public:
    enum EState
    {
        Reading,
        Writing
    };

    Archive(EState state)
      : m_state(state)
    {}

    Archive(const Archive&) = default;
    Archive& operator=(const Archive&) = default;
    ~Archive()                         = default;

    constexpr Archive& operator()(uint8* value, const char* name, const char* label) { return ar; }

    template<class ItemType, class SizeType>
    static constexpr RangedArchiveBlob RangedBytes(const ItemType* ptr, SizeType size)
    {
        return {reinterpret_cast<const void*>(ptr), size * sizeof(ItemType)};
    }

    constexpr bool IsReading() const { return m_state == Reading; }

    constexpr bool IsWriting() const { return m_state == Writing; }

    virtual void Serialize(void* data, size_t sizeInBytes, const char* name, const char* label) {}

  protected:
    EState m_state;
};