#pragma once

#include "Types.h"

class String;

struct FormatterArgument
{
    enum EType
    {
        DECIMAL,
        UNSIGNED,
        DOUBLE,
        STRING_POINTER
    };

    EType Type;

    union
    {
        int64        Decimal;
        uint64       Unsigned;
        double       Double;
        const tchar* StringPointer;
    };

    FormatterArgument(const int32 val);
    FormatterArgument(const uint32 val);
    FormatterArgument(const int64 val);
    FormatterArgument(const uint64 val);
    FormatterArgument(const float val);
    FormatterArgument(const double val);
    FormatterArgument(const tchar* val);
    FormatterArgument(const FormatterArgument& other);

  private:
    FormatterArgument();
};