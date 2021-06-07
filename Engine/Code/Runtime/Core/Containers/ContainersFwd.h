#pragma once

template<class>
class TDynamicAllocator;

using DefaultDynamicAllocator = TDynamicAllocator<unsigned int>; // 32 bits

template<class T, class Allocator = DefaultDynamicAllocator>
class TArray;
