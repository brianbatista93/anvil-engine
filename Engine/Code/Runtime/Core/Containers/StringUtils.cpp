#include "Containers/ContainersFwd.h"

#include "StringUtils.h"
#include <string>

struct ArgumentPosition
{
    uint32 Start;
    uint32 End;
    char   Arg[8] {0};
};

bool
ValidateFormat(const tchar* format)
{
    uint32 counter = 0;
    bool   valid   = false;
    while (*format) {
        tchar c = *format++;

        if (c == ATEXT('{')) {
            counter++;
        } else if (c == ATEXT('}')) {
            counter--;
        }
    }

    valid = counter == 0;

    return valid;
}

bool
IsDigit(tchar c)
{
    return c > 47 && c < 58;
}

TArray<ArgumentPosition>
GetArgsPositions(const tchar* format)
{
    TArray<ArgumentPosition> positions;

    uint32           index = 0;
    ArgumentPosition pos;
    tchar*           argPtr          = nullptr;
    bool             isOpen          = false;
    bool             isSpecification = false;

    while (*format) {
        tchar c = *format++;

        if (c == ATEXT('{')) {
            pos.Start = index;
            argPtr    = pos.Arg;
            isOpen    = true;
        } else if (isOpen) {
            if (c == ATEXT('}')) {
                pos.End = index;
                positions.Add(pos);
                argPtr = nullptr;
                isOpen = false;
            } else if (c == ATEXT(':')) {
                isSpecification = true;
            } else {
                *(argPtr++) = c;
            }
        }

        index++;
    }

    return positions;
}

TArray<FormatterArgment>
GetFormatterArgs(uint32 argc, const FormatterArgment* argv)
{
    return TArray<FormatterArgment>(argv, argv + argc);
}

bool
CheckArguments(TArray<ArgumentPosition>& positions, TArray<FormatterArgment>& args)
{
    if (positions.GetSize() != args.GetSize()) {
        return false;
    }

    for (uint32 i = 0; i < args.GetSize(); i++) {
        ArgumentPosition& pos = positions[i];
        if (*pos.Arg) {
            // FormatterArgment& arg = args[i];
            // TODO. Implement a real check.
        }
    }

    return true;
}

String
RemoveStringArgs(const tchar* format, TArray<ArgumentPosition>& positions)
{
    String f(format);
    uint32 offset = 0;

    for (ArgumentPosition& p : positions) {
        p.Start -= offset;
        p.End -= offset;
        const uint32 size = p.End - p.Start + 1;
        f.RemoveAt(p.Start, size);
        offset += size;
    }

    return f;
}

String
ArgumentToString(FormatterArgment arg)
{
    switch (arg.Type) {
        case FormatterArgment::DECIMAL:
            return std::to_string(arg.Decimal).c_str();
        case FormatterArgment::UNSIGNED:
            return std::to_string(arg.Unsigned).c_str();
        case FormatterArgment::DOUBLE:
            return std::to_string(arg.Double).c_str();
        case FormatterArgment::STRING_POINTER:
            return arg.StringPointer;
    }

    return ATEXT("");
}

bool
ProcessFormat(String& format, const TArray<ArgumentPosition>& positions, const TArray<FormatterArgment>& args)
{
    uint32 offset = 0;
    String temp;

    for (uint32 i = 0; i < args.GetSize(); i++) {
        auto& p = positions[i];
        auto& a = args[i];

        String sub = format.SubStr(offset, p.Start);
        sub += ArgumentToString(a);
        temp += sub;
        offset = p.Start;
    }

    format = temp;

    return true;
}

String
StringUtils::FormatStringInternal(const tchar* format, uint32 argc, const FormatterArgment* argv)
{
    AE_ASSERT(format && *format);

    AE_ASSERT(ValidateFormat(format));

    TArray<ArgumentPosition> argsPositions = GetArgsPositions(format);

    AE_ASSERT(argc == argsPositions.GetSize());

    if (!argsPositions.IsEmpty()) {
        TArray<FormatterArgment> args = GetFormatterArgs(argc, argv);

        AE_ASSERT(CheckArguments(argsPositions, args));

        String sformat = RemoveStringArgs(format, argsPositions);

        bool processed = ProcessFormat(sformat, argsPositions, args);
        AE_ASSERT(processed);
        if (processed) {
            return sformat;
        }
    }

    return ATEXT("");
}