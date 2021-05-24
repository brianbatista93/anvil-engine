#pragma once

#include "TypeTraits.h"

class CRC32
{
  public:
    static uint32 Get(uint8* p, uint32 n);
};

template<class T>
struct TCRC32
{
    uint32 operator()(const T& value) { return CRC32::Get(&value, (uint32)sizeof(T)); }
};