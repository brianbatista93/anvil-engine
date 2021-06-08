#pragma once

#include "Archive.h"
#include "Containers/Array.h"

class MemoryWriter : public Archive
{
  public:
    explicit MemoryWriter(TArray<uint8>& memoryBlock)
      : Archive(Writing)
      , m_data(memoryBlock)
      , m_offset(0)
    {}

    using Archive::operator();

    virtual void Serialize(void* data, size_t sizeInBytes, const char* name, const char* label) override
    {
        const uint64 bytes = m_offset + sizeInBytes - m_data.GetSize();
        if (bytes) {
            m_data.AddSlots(bytes);
        }

        AE_ASSERT((m_offset + sizeInBytes) <= m_data.GetSize());

        if (sizeInBytes) {
            memcpy(&m_data[m_offset], data, sizeInBytes);
            m_offset += sizeInBytes;
        }
    }

  private:
    TArray<uint8>& m_data;
    uint64         m_offset;
};