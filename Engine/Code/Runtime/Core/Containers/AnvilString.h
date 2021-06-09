#include "Array.h"
#include "CharacterConverter.h"

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
     * @brief Construct a string from a character pointer.
    */
    template<class CharType, typename = typename TEnableIf<TIsChar<CharType>::Value>::Type>
    constexpr String(const CharType* str)
    {
        if (str && *str) {
            TCharacterConverter<CharType, tchar, SizeType> conv;
            SizeType                                       srcLength = conv.GetConvertedLength(str) + 1;
            SizeType                                       dstLength;
            conv.Convert(nullptr, &dstLength, str, srcLength);
            m_data.AddSlots(dstLength);
            conv.Convert(m_data.GetData(), &dstLength, str, srcLength);
        }
    }

    /**
     * @brief Returns the size in bytes that this string is occuping on memory.
     * It includes the null-terminator and all indefined characters.
     * @return The size in bytes.
    */
    constexpr size_t GetSizeInBytes() const { return m_data.GetSizeInBytes(); }

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
    constexpr bool IsEmpty() const { return GetSize() == 0; }

  private:
    ArrayType m_data;
};