#include "StringFormatArgument.h"

// clang-format off
FormatterArgument::FormatterArgument(const int32 val)   : Type(DECIMAL), Decimal(val) {}
FormatterArgument::FormatterArgument(const uint32 val)  : Type(UNSIGNED), Unsigned(val) {}
FormatterArgument::FormatterArgument(const int64 val)   : Type(UNSIGNED), Decimal(val) {}
FormatterArgument::FormatterArgument(const uint64 val)  : Type(UNSIGNED), Unsigned(val) {}
FormatterArgument::FormatterArgument(const float val)   : Type(DOUBLE), Double(val) {}
FormatterArgument::FormatterArgument(const double val)  : Type(DOUBLE), Double(val) {}
FormatterArgument::FormatterArgument(const tchar* val)  : Type(STRING_POINTER), StringPointer(val) {}
// clang-format on
FormatterArgument::FormatterArgument(const FormatterArgument& other)
{
    Type = other.Type;
    switch (Type) {
        case FormatterArgument::DECIMAL:
            Decimal = other.Decimal;
            break;
        case FormatterArgument::UNSIGNED:
            Unsigned = other.Unsigned;
            break;
        case FormatterArgument::DOUBLE:
            Double = other.Double;
            break;
        case FormatterArgument::STRING_POINTER:
            StringPointer = other.StringPointer;
            break;
    }
}