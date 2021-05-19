#include "StringFormatArgument.h"

// clang-format off
FormatterArgment::FormatterArgment(const int32 val)   : Type(DECIMAL), Decimal(val) {}
FormatterArgment::FormatterArgment(const uint32 val)  : Type(UNSIGNED), Unsigned(val) {}
FormatterArgment::FormatterArgment(const int64 val)   : Type(UNSIGNED), Decimal(val) {}
FormatterArgment::FormatterArgment(const uint64 val)  : Type(UNSIGNED), Unsigned(val) {}
FormatterArgment::FormatterArgment(const float val)   : Type(DOUBLE), Double(val) {}
FormatterArgment::FormatterArgment(const double val)  : Type(DOUBLE), Double(val) {}
FormatterArgment::FormatterArgment(const tchar* val)  : Type(STRING_POINTER), StringPointer(val) {}
// clang-format on
FormatterArgment::FormatterArgment(const FormatterArgment& other)
{
    Type = other.Type;
    switch (Type) {
        case FormatterArgment::DECIMAL:
            Decimal = other.Decimal;
            break;
        case FormatterArgment::UNSIGNED:
            Unsigned = other.Unsigned;
            break;
        case FormatterArgment::DOUBLE:
            Double = other.Double;
            break;
        case FormatterArgment::STRING_POINTER:
            StringPointer = other.StringPointer;
            break;
    }
}