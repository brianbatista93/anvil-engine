#pragma once

template<bool Predicate, class Result = void>
struct TEnableIf
{};

template<class Result>
struct TEnableIf<true, Result>
{
    using Type = Result;
};

template<class Result>
struct TEnableIf<false, Result>
{};

// clang-format off
template<class T>    struct TIsChar             { enum { Value = false }; };
template<>           struct TIsChar<char>       { enum { Value = true }; };
#ifdef __cpp_char8_t
    template<>           struct TIsChar<char8>      { enum { Value = true }; };
#endif
template<>           struct TIsChar<char16>     { enum { Value = true }; };
template<>           struct TIsChar<char32>     { enum { Value = true }; };
template<>           struct TIsChar<wchar_t>    { enum { Value = true }; };
// clang-format on

template<class T0, class T1>
struct TIsSame
{
    enum
    {
        Value = false
    };
};

template<class T>
struct TIsSame<T, T>
{
    enum
    {
        Value = true
    };
};

template<typename T>
struct TIdentity
{
    typedef T Type;
};