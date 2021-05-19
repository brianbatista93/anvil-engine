#pragma once

#include "AnvilString.h"
#include "Types.h"

class StringUtils
{
  public:
    static String FormatStringInternal(const tchar* format, uint32 argc, const FormatterArgment* argv);

    template<class... Args>
    static String Format(const tchar* format, Args&&... args)
    {
        const uint32           size      = sizeof...(args);
        const FormatterArgment res[size] = {args...};

        return FormatStringInternal(format, size, res);
    }
};