#pragma once

#include "Archive.h"
#include "Containers/Array.h"

class MemoryReader : public Archive
{
  public:
    explicit MemoryReader(const TArray<uint8>& memoryBlock)
      : Archive(Reading)
      , m_data(memoryBlock)
      , m_offset(0)
    {}

    virtual void Serialize(void* data, size_t sizeInBytes, const char* name, const char* label) override
    {
        if (sizeInBytes) {
            memcpy(data, &m_data[m_offset], sizeInBytes);
            m_offset += sizeInBytes;
        }
    }

  private:
    const TArray<uint8>& m_data;
    uint64               m_offset;
};