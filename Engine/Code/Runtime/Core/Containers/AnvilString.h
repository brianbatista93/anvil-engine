#pragma once

#include "Array.h"
#include "CharacterConverter.h"

class String;

tchar*
GetData(String&);

const tchar*
GetData(const String&);

class String
{
  public:
    using ItemType  = tchar;
    using ArrayType = TArray<ItemType, THeapAllocator<uint32>>;
    using SizeType  = ArrayType::SizeType;

    static constexpr SizeType NPos = ~0u;

    constexpr String()              = default;
    constexpr String(const String&) = default;
    constexpr String(String&&)      = default;
    String& operator=(const String&) = default;
    String& operator=(String&&) = default;
    ~String()                   = default;

    /**
     * @brief Copy a string with extra space.
     * @param other String to be copied.
     * @param extraSpace Extra space.
    */
    constexpr explicit String(const String& other, SizeType extraSpace)
      : m_data(other.m_data, extraSpace + ((other.m_data.GetSize() || !extraSpace) ? 0 : 1))
    {}

    /**
     * @brief Move a string with extra space.
     * @param other String to be moved.
     * @param extraSpace Extra space.
    */
    constexpr explicit String(String&& other, SizeType extraSpace)
      : m_data(std::move(other.m_data), extraSpace + ((other.m_data.GetSize() || !extraSpace) ? 0 : 1))
    {}

    /**
     * @brief Construct a string from a character pointer.
    */
    template<class CharType, typename = typename TEnableIf<TIsChar<CharType>::Value>::Type>
    constexpr String(const CharType* str)
    {
        if (str && *str) {
            TCharacterConverter<CharType, tchar, SizeType> conv;
            SizeType                                       srcLength = conv.GetConvertedLength(str) + 1;
            SizeType                                       dstLength = 0;
            conv.Convert(nullptr, &dstLength, str, srcLength);
            m_data.AddSlots(dstLength);
            conv.Convert(m_data.GetData(), &dstLength, str, srcLength);
        }
    }

    /**
     * @brief Copy assingment from tchar
     * @param other Tchar pointer
    */
    //constexpr String& operator=(const tchar* other)
    //{
    //    if (m_data.GetData() != other) {
    //        TCharacterConverter<tchar, tchar, SizeType> conv;
    //        SizeType                                    length = (other && *other) ? (conv.GetConvertedLength(other) + 1) : 0;
    //        m_data.Clear(true);
    //        m_data.AddSlots(length);

    //        if (length) {
    //            memcpy(m_data.GetData(), other, length * sizeof(tchar));
    //        }
    //    }

    //    return *this;
    //}

    /**
     * @brief Returns the size in bytes that this string is occuping on memory.
     * It includes the null-terminator and all indefined characters.
     * @return The size in bytes.
    */
    constexpr size_t GetSizeInBytes() const { return m_data.GetSizeInBytes(); }

    [[nodiscard]] constexpr ArrayType& GetArray() { return m_data; }

    [[nodiscard]] constexpr const ArrayType& GetArray() const { return m_data; }

    /**
     * @brief Returns the number of characters on the string including the null-terminator.
     * For the string length use GetLength.
     * @return The number of characters.
    */
    constexpr SizeType GetSize() const { return m_data.GetSize(); }

    /**
     * @brief Returns the string length.
     * @return The string length. 
    */
    constexpr SizeType GetLength() const { return GetSize() ? (GetSize() - 1) : 0; }

    /**
     * @brief Checks if the string is empty.
     * It does not mean the string has not allocated memory.
     * @return True if empty, false otherwise.
    */
    constexpr bool IsEmpty() const { return GetSize() <= 1; }

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
    constexpr ItemType& operator[](SizeType index) { return m_data.GetData()[index]; }

    /**
     * @brief Gets one character at an index.
     * @param index Index of the character
     * @return The character at an index.
    */
    constexpr const ItemType& operator[](SizeType index) const { return m_data.GetData()[index]; }

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

    void AppendCharacters(const char* chrs, SizeType count);
    void AppendCharacters(const char16* chrs, SizeType count);
    void AppendCharacters(const wchar_t* chrs, SizeType count);

    constexpr String& AppendCharacter(tchar chr)
    {
        if (chr) {
            SizeType index = (m_data.GetSize() > 0) ? m_data.GetSize() - 1 : 0;
            SizeType count = (m_data.GetSize() > 0) ? 1 : 2;

            m_data.AddSlots(count);
            m_data[index]     = chr;
            m_data[index + 1] = 0;
        }

        return *this;
    }

    template<class CharType>
    constexpr String& Append(const CharType* chrs, SizeType count)
    {
        AppendCharacters(chrs, count);
        return *this;
    }

    template<class CharType>
    constexpr String& Append(CharType* chrs)
    {
        TCharacterConverter<CharType, ItemType, SizeType> conv;
        const SizeType                                    count = conv.GetConvertedLength(chrs);
        AppendCharacters(chrs, count);
        return *this;
    }

    template<class CharType>
    constexpr String& Append(CharType&& chrs)
    {
        AppendCharacters(GetData(chrs), ::GetSize(chrs));
        return *this;
    }

    constexpr String& operator+=(char chr) { return AppendCharacter(static_cast<tchar>(chr)); }
    constexpr String& operator+=(char16 chr) { return AppendCharacter(static_cast<tchar>(chr)); }
    constexpr String& operator+=(wchar_t chr) { return AppendCharacter(static_cast<tchar>(chr)); }

    template<class StringType>
    constexpr String& operator+=(StringType&& str)
    {
        return Append(std::forward<StringType>(str));
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
    ArrayType m_data;
};

inline tchar*
GetData(String& str)
{
    return str.GetArray().GetData();
}

inline const tchar*
GetData(const String& str)
{
    return str.GetArray().GetData();
}

inline uint32
GetSize(const String& str)
{
    return str.GetLength();
}