#pragma once

#include "Types.h"

class String;

struct FormatterArgment
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


    FormatterArgment(const int32 val);
    FormatterArgment(const uint32 val);
    FormatterArgment(const int64 val);
    FormatterArgment(const uint64 val);
    FormatterArgment(const float val);
    FormatterArgment(const double val);
    FormatterArgment(const tchar* val);
    FormatterArgment(const FormatterArgment& other);

  private:
    FormatterArgment();
};