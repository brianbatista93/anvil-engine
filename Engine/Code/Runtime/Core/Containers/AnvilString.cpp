#include "AnvilString.h"

template<typename CharType>
void
AppendCharactersImpl(String::ArrayType& result, const CharType* chrs, String::SizeType count)
{
    AE_ASSERT(count >= 0);

    if (!count) {
        return;
    }

    AE_ASSERT(chrs);

    const int32 oldNum = result.GetSize();

    result.AddSlots(count + (oldNum ? 0 : 1));

    tchar* dest = result.GetData() + oldNum - (oldNum ? 1 : 0);

    TCharacterConverter<CharType, tchar, String::SizeType> conv;
    conv.Convert(dest, &count, chrs, count);

    dest[count] = 0;
}

void
String::AppendCharacters(const char* chrs, SizeType count)
{
    AppendCharactersImpl(m_data, chrs, count);
}

void
String::AppendCharacters(const char16* chrs, SizeType count)
{
    AppendCharactersImpl(m_data, chrs, count);
}

void
String::AppendCharacters(const wchar_t* chrs, SizeType count)
{
    AppendCharactersImpl(m_data, chrs, count);
}