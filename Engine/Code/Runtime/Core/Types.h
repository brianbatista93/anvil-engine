#pragma once

using uint8  = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long long;

using int8  = signed char;
using int16 = signed short;
using int32 = signed int;
using int64 = signed long long;

#ifdef __cpp_char8_t
using char8 = char8_t;
#else
using char8 = char;
#endif

using char16 = char16_t;
using char32 = char32_t;

using tchar = char8;

#ifdef ATEXT
    #undef FTEXT
#endif // FTEXT

#define ATEXT(t) u8##t