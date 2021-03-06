#pragma once

#include "string.h"
#include <algorithm>
#include <assert.h>
#include <cstdlib>
#include <memory>
#include <type_traits>
#include <utility>

#ifdef AE_DEBUG
    #if AE_WINDOWS
        #define _CRTDBG_MAP_ALLOC
        #include <crtdbg.h>

        #define DEBUG_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
        #define anew      DEBUG_NEW
    #endif
#endif

constexpr void
SafeDelete(void* ptr)
{
    if (ptr) {
        delete ptr;
        ptr = nullptr;
    }
}

constexpr void
SafeDeleteArray(void* ptr)
{
    if (ptr) {
        delete[] ptr;
        ptr = nullptr;
    }
}

#ifdef AE_DEBUG
    #define AE_ASSERT(x) assert(x)
#else
    #define AE_ASSERT(x)
#endif // AE_DEBUG
