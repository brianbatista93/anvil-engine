#pragma once

#include "Array.h"
#include "Misc/CRC32.h"
#include "StringFormatArgument.h"
#include "TypeTraits.h"

class String;

tchar*
GetData(String& str);

const tchar*
GetData(const String& str);

uint32
GetSize(const String& str);

/**
 * @brief String utils methods depending on the character's type.
*/
template<class CharType, typename = typename TEnableIf<TIsChar<CharType>::Value>::Type>
class TCStringOperations
{
  public:
    template<class OtherCharType, typename = typename TEnableIf<TIsChar<OtherCharType>::Value>::Type>
    static constexpr uint32 ConvertString(CharType* dst, uint32 dstLength, const OtherCharType* src, uint32 srcLength)
    {
        if constexpr (TIsSame<CharType, OtherCharType>::Value) {
            if (dst) {
                memcpy(dst, src, dstLength * sizeof(CharType));
            }
            return srcLength;
        }

        uint32                    size      = dstLength <= srcLength ? dstLength : srcLength;
        uint32                    remaining = (srcLength * sizeof(OtherCharType));
        uint32                    offset    = 0;
        CharType*                 ptr       = dst;
        std::unique_ptr<CharType> uptr;
        if (dst == nullptr) {
            ptr = new CharType[dstLength];
            uptr.reset(ptr);
        }

        const CharType* begin = ptr;

        for (uint32 c = 0; c < size; c++) {
            uint32 codePoint = static_cast<uint32>(src[c]);
            remaining -= sizeof(OtherCharType);

            if (codePoint < 0x80) {
                *(ptr++) = (CharType)codePoint;
                offset++;
            } else if (codePoint < 0x800) {
                if (remaining >= 2) {
                    *(ptr++) = (CharType)((codePoint >> 6) | 128 | 64);
                    *(ptr++) = (CharType)(codePoint & 0x3F) | 128;
                    offset += 2;
                } else {
                    *(ptr++) = (CharType)'?';
                    offset++;
                }
            } else if (codePoint < 0x10000) {
                if (remaining >= 3) {
                    *(ptr++) = (CharType)((codePoint >> 12) | 128 | 64 | 32);
                    *(ptr++) = (CharType)((codePoint >> 6) & 0x3F) | 128;
                    *(ptr++) = (CharType)(codePoint & 0x3F) | 128;
                    offset += 3;
                } else {
                    *(ptr++) = (CharType)'?';
                    offset++;
                }
            } else {
                if (remaining >= 4) {
                    *(ptr++) = (CharType)((codePoint >> 18) | 128 | 64 | 32 | 16);
                    *(ptr++) = (CharType)((codePoint >> 12) & 0x3F) | 128;
                    *(ptr++) = (CharType)((codePoint >> 6) & 0x3F) | 128;
                    *(ptr++) = (CharType)(codePoint & 0x3F) | 128;
                    offset += 4;
                } else {
                    *(ptr++) = (CharType)'?';
                    offset++;
                }
            }
        }

        uint32 written = static_cast<uint32>(ptr - begin);

        return written;
    }

    template<class OtherCharType, typename = typename TEnableIf<TIsChar<OtherCharType>::Value>::Type>
    static constexpr uint32 GetConvertedLength(const OtherCharType* src, uint32 srcLength)
    {
        return ConvertString(nullptr, srcLength * 4, src, srcLength);
    }
};

/**
 * @brief Container storing contiguous dynamic array of characters.
*/
class String
{
  public:
    using ItemType = tchar;

    static constexpr uint32 NPos = ~0u;

    constexpr String()              = default;
    constexpr String(const String&) = default;
    constexpr String(String&&)      = default;
    String& operator=(const String&) = default;
    String& operator=(String&&) = default;
    ~String()                   = default;

    /**
     * @brief Construct a string from a character pointer.
    */
    template<class CharType, typename = typename TEnableIf<TIsChar<CharType>::Value>::Type>
    constexpr String(const CharType* str)
    {
        if (str && *str) {
            const uint32 strLength  = (uint32)std::char_traits<CharType>::length(str) + 1;
            const uint32 convLength = TCStringOperations<tchar>::GetConvertedLength<CharType>(str, strLength);

            m_data.Resize(convLength);

            TCStringOperations<tchar>::ConvertString<CharType>(GetData(), convLength, str, strLength);
        }
    }

    constexpr String(const ItemType* begin, const ItemType* end)
      : m_data(begin, end)
    {
        if (m_data[GetSize() - 1] != 0) {
            m_data.AddSlots(1);
        }
    }

    /**
     * @brief Returns the string's data pointer.
     * @return The string's data pointer.
    */
    constexpr ItemType* GetData() { return m_data.GetData(); }

    /**
     * @brief Returns the string's data pointer.
     * @return The string's data pointer.
    */
    constexpr const ItemType* GetData() const { return m_data.GetData(); }

    /**
     * @brief Returns the size in bytes that this string is occuping on memory.
     * It includes the null-terminator and all indefined characters.
     * @return The size in bytes.
    */
    constexpr uint64 GetSizeInBytes() const { return m_data.GetCapacity() * sizeof(ItemType); }

    /**
     * @brief Returns the number of characters on the string including the null-terminator.
     * For the string length use GetLength.
     * @return The number of characters.
    */
    constexpr uint32 GetSize() const { return m_data.GetSize(); }

    /**
     * @brief Returns the string length.
     * @return The string length. 
    */
    constexpr uint32 GetLength() const { return GetSize() ? (GetSize() - 1) : 0; }

    /**
     * @brief Checks if the string is empty.
     * It does not mean the string has not allocated memory.
     * @return True if empty, false otherwise.
    */
    constexpr bool IsEmpty() const { return GetSize() == 0; }

    /**
     * @brief Returns the string as const tchar*.
     * @return Array of tchars.
    */
    constexpr const ItemType* operator*() const { return m_data.GetData(); }

    /**
     * @brief Gets one character at an index.
     * @param index Index of the character
     * @return The character at an index.
    */
    constexpr ItemType& operator[](uint32 index) { return m_data.GetData()[index]; }

    /**
     * @brief Gets one character at an index.
     * @param index Index of the character
     * @return The character at an index.
    */
    constexpr const ItemType& operator[](uint32 index) const { return m_data.GetData()[index]; }

    /**
     * @brief C++ iterator begin.
     * @return A pointer to the begin.
    */
    constexpr ItemType* begin() noexcept { return m_data.begin(); }

    /**
     * @brief C++ iterator begin.
     * @return A pointer to the begin.
    */
    constexpr const ItemType* begin() const noexcept { return m_data.begin(); }

    /**
     * @brief C++ iterator end.
     * @return A pointer to the end.
    */
    constexpr ItemType* end() noexcept
    {
        auto end = m_data.end();
        if (GetSize()) {
            --end;
        }
        return end;
    }

    /**
     * @brief C++ iterator end.
     * @return A pointer to the end.
    */
    constexpr const ItemType* end() const noexcept
    {
        auto end = m_data.end();
        if (GetSize()) {
            --end;
        }
        return end;
    }

    /**
     * @brief Removes a count of characters starting from a index.
     * @param index Starting index.
     * @param count Number of characters to remove.
    */
    constexpr void RemoveAt(uint32 index, uint32 count = 1) { m_data.RemoveAt(index, count); }

    String SubStr(uint32 startIndex, uint32 endIndex = NPos) const
    {
        AE_ASSERT(endIndex != NPos && endIndex < GetSize());

        const uint32 offset = (endIndex == NPos) ? GetLength() : endIndex;

        return String(begin() + startIndex, begin() + offset);
    }

    /**
     * @brief Appends an array of count characters to the end of the string.
     * @param chrs Array of characters to be appended.
     * @param count Number of character to append.
    */
    void AppendChars(const char* chrs, uint32 count);

#ifdef __cpp_char8_t
    /**
     * @brief Appends an array of count characters to the end of the string.
     * @param chrs Array of characters to be appended.
     * @param count Number of character to append.
    */
    void AppendChars(const char8* chrs, uint32 count);
#endif

    /**
     * @brief Appends an array of count characters to the end of the string.
     * @param chrs Array of characters to be appended.
     * @param count Number of character to append.
    */
    void AppendChars(const char16* chrs, uint32 count);

    /**
     * @brief Appends an array of count characters to the end of the string.
     * @param chrs Array of characters to be appended.
     * @param count Number of character to append.
    */
    void AppendChars(const char32* chrs, uint32 count);

    /**
     * @brief Appends an array of count characters to the end of the string.
     * @param chrs Array of characters to be appended.
     * @param count Number of character to append.
    */
    void AppendChars(const wchar_t* chrs, uint32 count);

    template<typename CharRange>
    constexpr String& Append(CharRange&& chrs)
    {
        AppendChars(::GetData(chrs), ::GetSize(chrs));
        return *this;
    }

    template<typename Str>
    constexpr String& operator+=(Str&& chrs)
    {
        return Append(std::forward<Str>(chrs));
    }

    /**
     * @brief Checks if a string rhs is equals to this one.
     * @param rhs The string to be compared with.
     * @return True if both strings are equal.
    */
    constexpr bool Equals(const String& rhs) const { return m_data == rhs.m_data; }

    [[nodiscard]] friend bool operator==(const String& lhs, const String& rhs) { return lhs.Equals(rhs); }

    [[nodiscard]] friend bool operator!=(const String& lhs, const String& rhs) { return !(lhs == rhs); }

    [[nodiscard]] friend bool operator<(const String& lhs, const String& rhs) { return lhs.m_data < rhs.m_data; }

    [[nodiscard]] friend bool operator<=(const String& lhs, const String& rhs) { return lhs.m_data <= rhs.m_data; }

    [[nodiscard]] friend bool operator>(const String& lhs, const String& rhs) { return lhs.m_data > rhs.m_data; }

    [[nodiscard]] friend bool operator>=(const String& lhs, const String& rhs) { return lhs.m_data >= rhs.m_data; }

  private:
    template<class Lhs, class Rhs>
    static String ConcatStrings(typename TIdentity<Lhs>::Type lhs, typename TIdentity<Rhs>::Type rhs)
    {
        const uint32 lhsLength = lhs.GetLength();
        const uint32 rhsLength = rhs.GetLength();

        if (lhsLength == 0) {
            return std::move(rhs);
        }

        String result(std::move(lhs), rhsLength);
        result.AppendChars(rhs.GetData(), rhsLength);

        return result;
    }

  private:
    TArray<ItemType, TDynamicAllocator<ItemType, uint32>> m_data;
};

inline tchar*
GetData(String& str)
{
    return str.GetData();
}

inline const tchar*
GetData(const String& str)
{
    return str.GetData();
}

inline uint32
GetSize(const String& str)
{
    return str.GetLength();
}

template<>
struct TCRC32<String>
{
    using ResultType = uint32;
    ResultType operator()(const String& value) const { return CRC32::Get((uint8*)*value, value.GetLength() * sizeof(String::ItemType)); }
};