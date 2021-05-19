#include "AnvilString.h"

template<typename Char>
void
AppendCharsInternal(TArray<tchar>& result, const Char* chrs, uint32 count)
{
    if (count == 0) {
        return;
    }

    AE_ASSERT(chrs);

    const uint32 oldSize = result.GetSize();

    result.AddSlots(count + (oldSize ? 0 : 1));

    tchar* dst = result.GetData() + oldSize - (oldSize ? 1 : 0);

    TCStringOperations<tchar>::ConvertString(dst, count, chrs, count);

    dst[count] = 0;
}

void
String::AppendChars(const char* chrs, uint32 count)
{
    AppendCharsInternal(m_data, chrs, count);
}

#ifdef __cpp_char8_t
void
String::AppendChars(const char8* chrs, uint32 count)
{
    AppendCharsInternal(m_data, chrs, count);
}
#endif

void
String::AppendChars(const char16* chrs, uint32 count)
{
    AppendCharsInternal(m_data, chrs, count);
}

void
String::AppendChars(const char32* chrs, uint32 count)
{
    AppendCharsInternal(m_data, chrs, count);
}

void
String::AppendChars(const wchar_t* chrs, uint32 count)
{
    AppendCharsInternal(m_data, chrs, count);
}