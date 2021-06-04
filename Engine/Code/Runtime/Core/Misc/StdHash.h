#pragma once

template<class T>
struct TStdHash
{
    using ResultType = size_t;
    ResultType operator()(const T& value) { return std::hash<T>()(value); }
};