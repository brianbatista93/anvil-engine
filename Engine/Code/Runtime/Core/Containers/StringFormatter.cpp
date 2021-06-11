#include "Containers/ContainersFwd.h"

#include "AnvilString.h"

struct ArgumentPosition
{
    String::SizeType Start;
    String::SizeType End;
    bool             HasIndex     = false;
    bool             HasExtraInfo = false;
    tchar            Arg[8] {0};
    String::SizeType Index = 0;
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

    String::SizeType index = 0;
    ArgumentPosition pos;
    tchar*           argPtr = nullptr;
    bool             isOpen = false;

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
                pos.HasExtraInfo = true;
            } else if (IsDigit(c)) {
                if (!pos.HasExtraInfo) {
                    pos.HasIndex = true;
                    pos.Index    = static_cast<String::SizeType>(c - 48);
                }
            } else {
                *(argPtr++) = c;
            }
        }

        index++;
    }

    return positions;
}

TArray<FormatterArgument>
GetFormatterArgs(uint32 argc, const FormatterArgument* argv)
{
    return TArray<FormatterArgument>(argv, argv + argc);
}

bool
CheckArguments(TArray<ArgumentPosition>& positions, TArray<FormatterArgument>& args)
{
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
ArgumentToString(FormatterArgument arg)
{
    switch (arg.Type) {
        case FormatterArgument::DECIMAL:
            return ToString(arg.Decimal);
        case FormatterArgument::UNSIGNED:
            return ToString(arg.Unsigned);
        case FormatterArgument::DOUBLE:
            return ToString(arg.Double);
        case FormatterArgument::STRING_POINTER:
            return arg.StringPointer;
    }

    return ATEXT("");
}

bool
ProcessFormat(String& format, const TArray<ArgumentPosition>& positions, const TArray<FormatterArgument>& args)
{
    String::SizeType offset = 0;
    String           temp;
    const auto       argsCount = args.GetSize();

    String::SizeType argsIndex = 0;

    for (auto i = 0; i < positions.GetSize(); i++) {
        ArgumentPosition  p = positions[i];
        FormatterArgument a = args[argsIndex];
        if (p.HasIndex) {
            AE_ASSERT(p.Index < args.GetSize());
            a = args[p.Index];
        } else {
            argsIndex++;
        }

        String sub;
        if (p.Start - offset > 0)
            sub = format.SubStr(offset, p.Start);

        sub += ArgumentToString(a);
        temp += sub;
        offset = p.Start;
    }

    if (argsCount) {
        temp += format.SubStr(positions[argsCount - 1].End - 1);
    }

    format = temp;

    return true;
}

String
String::FormatStringInternal(const tchar* format, uint32 argc, const FormatterArgument* argv)
{
    AE_ASSERT(format && *format);

    AE_ASSERT(ValidateFormat(format));

    TArray<ArgumentPosition> argsPositions = GetArgsPositions(format);

    // AE_ASSERT(argc == argsPositions.GetSize());

    if (!argsPositions.IsEmpty()) {
        TArray<FormatterArgument> args = GetFormatterArgs(argc, argv);

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