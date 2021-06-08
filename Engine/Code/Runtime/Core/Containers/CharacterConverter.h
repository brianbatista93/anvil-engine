#pragma once

template<class _FromType, class _ToType, class _SizeType>
class TCharacterConverter
{
  public:
    using FromType = _FromType;
    using ToType   = _ToType;
    using SizeType = _SizeType;

    SizeType GetConvertedLength(const FromType* src) const { return 0; }

    ToType* Convert(ToType* dst, const FromType* src, SizeType srcLength) const { return nullptr; }
};

template<class Type, class _SizeType>
class TCharacterConverter<Type, Type, _SizeType>
{
  public:
    using FromType = Type;
    using ToType   = Type;
    using SizeType = _SizeType;

    SizeType GetConvertedLength(const FromType* src) const { return static_cast<SizeType>(std::char_traits<ToType>::length(src)); }

    ToType* Convert(ToType* dst, const FromType* src, SizeType srcLength) const
    {
        memcpy(dst, src, srcLength * sizeof(ToType));
        return dst;
    }
};

template<class _SizeType>
class TCharacterConverter<wchar_t, tchar, _SizeType>
{
  public:
    using FromType = wchar_t;
    using ToType   = tchar;
    using SizeType = _SizeType;

    SizeType GetConvertedLength(const FromType* src) const { return static_cast<SizeType>(std::char_traits<FromType>::length(src)); }

    ToType* Convert(ToType* dst, const FromType* src, SizeType srcLength) const
    {
        if constexpr (sizeof(FromType) == sizeof(ToType)) {
            memcpy(dst, src, srcLength * sizeof(ToType));
        }

        return dst;
    }
};

template<class _SizeType>
class TCharacterConverter<char, tchar, _SizeType>
{
  public:
    using FromType = char;
    using ToType   = tchar;
    using SizeType = _SizeType;

    SizeType GetConvertedLength(const FromType* src) const { return static_cast<SizeType>(std::char_traits<FromType>::length(src)); }

    ToType* Convert(ToType* dst, const FromType* src, SizeType srcLength) const
    {

        for (SizeType i = 0; i < srcLength; i++) {
            FromType srcChr = src[i];
            if ((srcChr >= 0x00 && srcChr <= 0x7F) && (FromType)(ToType)srcChr == srcChr) {
                dst[i] = static_cast<ToType>(srcChr);
            } else {
                dst[i] = src[i] << 8;
                dst[i] = src[++i];
            }
        }

        return dst;
    }
};