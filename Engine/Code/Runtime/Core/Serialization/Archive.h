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

    inline Archive& operator()(uint8& value, const char* name, const char* label) { return SerializeOrdered(value, name, label); }
    inline Archive& operator()(uint16& value, const char* name, const char* label) { return SerializeOrdered(value, name, label); }
    inline Archive& operator()(uint32& value, const char* name, const char* label) { return SerializeOrdered(value, name, label); }
    inline Archive& operator()(uint64& value, const char* name, const char* label) { return SerializeOrdered(value, name, label); }

    inline Archive& operator()(int8& value, const char* name, const char* label) { return SerializeOrdered(value, name, label); }
    inline Archive& operator()(int16& value, const char* name, const char* label) { return SerializeOrdered(value, name, label); }
    inline Archive& operator()(int32& value, const char* name, const char* label) { return SerializeOrdered(value, name, label); }
    inline Archive& operator()(int64& value, const char* name, const char* label) { return SerializeOrdered(value, name, label); }

    inline Archive& operator()(float& value, const char* name, const char* label) { return SerializeOrdered(value, name, label); }
    inline Archive& operator()(double& value, const char* name, const char* label) { return SerializeOrdered(value, name, label); }

    template<class T>
    inline Archive& operator()(T& object, const char* name, const char* label)
    {
        return object.Serialize(*this, name, label);
    }

    inline Archive& operator()(const RangedArchiveBlob& blob, const char* name, const char* label)
    {
        AE_ASSERT(IsWriting());
        AE_ASSERT(blob.IsValid());
        Serialize(const_cast<void*>(blob.Pointer), blob.Size, name, label);
        return *this;
    }

    template<class ItemType, class SizeType>
    inline static RangedArchiveBlob RangedBytes(const ItemType* ptr, SizeType size)
    {
        return {reinterpret_cast<const void*>(ptr), size * sizeof(ItemType)};
    }

    constexpr bool IsReading() const { return m_state == Reading; }

    constexpr bool IsWriting() const { return m_state == Writing; }

    virtual void Serialize(void* data, size_t sizeInBytes, const char* name, const char* label) {}

  private:
    template<class T>
    inline Archive& SerializeOrdered(T& value, const char* name, const char* label)
    {
        Serialize(reinterpret_cast<void*>(&value), sizeof(T), name, label);
        return *this;
    }

  protected:
    EState m_state;
};