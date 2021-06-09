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

#define SAFE_DELETE(ptr)                                                                                                                   \
    {                                                                                                                                      \
        if (ptr) {                                                                                                                         \
            delete ptr;                                                                                                                    \
            ptr = nullptr;                                                                                                                 \
        }                                                                                                                                  \
    }

#define SAFE_DELETE_ARRAY(ptr)                                                                                                             \
    {                                                                                                                                      \
        if (ptr) {                                                                                                                         \
            delete[] ptr;                                                                                                                  \
            ptr = nullptr;                                                                                                                 \
        }                                                                                                                                  \
    }

#ifdef AE_DEBUG
    #define AE_ASSERT(x) assert(x)
#else
    #define AE_ASSERT(x)
#endif // AE_DEBUG
