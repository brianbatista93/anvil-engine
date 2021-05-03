#pragma once

#include "Memory/Allocators/DynamicAllocator.h"

/**
 * @brief Container of contiguous memory.
*/
template<class T, class Alloc = TDynamicAllocator<T>>
class TArray
{
  public:
    TArray() {}

  private:
    Alloc m_allocator;
};