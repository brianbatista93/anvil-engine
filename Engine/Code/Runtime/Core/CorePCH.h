#pragma once

#include <assert.h>
#include <type_traits>

constexpr void
SafeDelete(void* ptr)
{
    if (ptr) {
        delete ptr;
        ptr = nullptr;
    }
}

#define AE_ASSERT(x) assert(x)