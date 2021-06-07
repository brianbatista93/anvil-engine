#pragma once

template<class, class>
class TDynamicAllocator;

template<class T>
using DefaultDynamicAllocator = TDynamicAllocator<T, unsigned int>; // 32 bits
template<class T>
using DefaultDynamicAllocator64 = TDynamicAllocator<T, unsigned long long>; // 64 bits

template<class T, class Allocator = DefaultDynamicAllocator64<T>>
class TArray;
