#pragma once

template<class>
class THeapAllocator;

using DefaultHeapAllocator   = THeapAllocator<unsigned int>;       // 32 bits
using DefaultHeapAllocator64 = THeapAllocator<unsigned long long>; // 64 bits

template<class T, class Allocator = DefaultHeapAllocator64>
class TArray;
