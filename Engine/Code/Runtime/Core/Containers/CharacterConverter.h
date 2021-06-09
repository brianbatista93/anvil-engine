#pragma once

constexpr const tchar bogusSymbol = ATEXT('?');

template<class _FromType, class _ToType, class _SizeType>
class TCharacterConverter
{
  public:
    using FromType = _FromType;
    using ToType   = _ToType;
    using SizeType = _SizeType;

    SizeType GetConvertedLength(const FromType* src) const { return 0; }

    ToType* Convert(ToType* dst, SizeType* dstLength, const FromType* src, SizeType srcLength) const { return nullptr; }
};

template<class Type, class _SizeType>
class TCharacterConverter<Type, Type, _SizeType>
{
  public:
    using FromType = Type;
    using ToType   = Type;
    using SizeType = _SizeType;

    SizeType GetConvertedLength(const FromType* src) const { return static_cast<SizeType>(std::char_traits<ToType>::length(src)); }

    ToType* Convert(ToType* dst, SizeType* dstLength, const FromType* src, SizeType srcLength) const
    {
        if (dst == nullptr) {
            *dstLength = srcLength;
            return nullptr;
        } else {
            memcpy(dst, src, srcLength * sizeof(ToType));
            return dst;
        }
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

    ToType* Convert(ToType* dst, SizeType* dstLength, const FromType* src, SizeType srcLength) const
    {
        if constexpr (sizeof(FromType) == sizeof(ToType)) {
            if (dst == nullptr) {
                *dstLength = srcLength;
                return nullptr;
            } else {
                memcpy(dst, src, srcLength * sizeof(ToType));
            }
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

    ToType* Convert(ToType* dst, SizeType* dstLength, const FromType* src, SizeType srcLength) const
    {
        FromType* srcPtr      = (FromType*)src;
        ToType*   dstPtr      = dst;
        SizeType  srcIterator = 0;
        SizeType  dstIterator = 0;

        if (dst == nullptr) {
            *dstLength = 0;
            dstPtr     = new ToType[srcLength * sizeof(ToType)];
        }

        while (*srcPtr) {
            if (*srcPtr >= 0 && *srcPtr < 0x80) {
                *dstPtr = (ToType)(*srcPtr);
            } else {
                *dstPtr = (ToType)bogusSymbol;
            }

            dstPtr++;
            srcPtr++;
            srcIterator++;
            dstIterator++;
        }

        if (*dstPtr != ATEXT('\0')) {
            *dstPtr = ATEXT('\0');
            dstPtr++;
            dstIterator++;
        }

        if (dst == nullptr) {
            dstPtr -= dstIterator;
            SAFE_DELETE_ARRAY(dstPtr);
            *dstLength = dstIterator;
        }

        return dst;
    }
};

template<class _SizeType>
class TCharacterConverter<char8, tchar, _SizeType>
{
  public:
    using FromType = char8;
    using ToType   = tchar;
    using SizeType = _SizeType;

    SizeType GetConvertedLength(const FromType* src) const { return static_cast<SizeType>(std::char_traits<FromType>::length(src)); }

    ToType* Convert(ToType* dst, SizeType* dstLength, const FromType* src, SizeType srcLength) const
    {
        FromType* srcPtr      = (FromType*)src;
        ToType*   dstPtr      = dst;
        SizeType  srcIterator = 0;
        SizeType  dstIterator = 0;

        if (dst == nullptr) {
            *dstLength = 0;
            dstPtr     = new ToType[srcLength * sizeof(ToType)];
        }

        while (*srcPtr && (srcIterator < srcLength)) {
            if (*srcPtr < 0x80) { // 0 - 127
                *dstPtr = (ToType)(*srcPtr);
                dstPtr++;
                dstIterator++;

                srcPtr++;
                srcIterator++;
            } else if (*srcPtr < 0xC0) { // 0x80 - 0xBF, Reserved for UTF-8 encoding.
                srcPtr++;
                srcIterator++;
            } else if (*srcPtr < 0xE0) { // 128 - 2047

                ToType high = (ToType)((*srcPtr++) & 0x1F);
                ToType low  = (ToType)((*srcPtr++) & 0x3F);

                int32 unicode = (high << 8) | low;

                if ((0 <= unicode && unicode <= 0xD7FF) || (0xE000 <= unicode && unicode <= 0xFFFF)) {
                    *dstPtr = (ToType)unicode;
                    dstPtr++;
                    dstIterator++;
                }

                srcIterator += 2;
            } else if (*srcPtr < 0xF0) { // 2048 - 65535

                ToType fourth     = (ToType)((*srcPtr++) & 0xF);
                ToType third      = (ToType)((*srcPtr) & 0x3C) >> 2;
                ToType secondHigh = (ToType)((*srcPtr++) & 0x3);
                ToType secondLow  = (ToType)((*srcPtr) & 0x30) >> 4;
                ToType first      = (ToType)((*srcPtr++) & 0xF);

                int32 unicode = (fourth << 12) | (third << 8) | (secondHigh << 6) | (secondLow << 4) | first;

                if ((0 <= unicode && unicode <= 0xD7FF) || (0xE000 <= unicode && unicode <= 0xFFFF)) {
                    //Directly set the value to the UTF-16 code unit.
                    *dstPtr = (ToType)unicode;
                    dstPtr++;
                    dstIterator++;
                }

                srcIterator += 3;
            } else if (*srcPtr < 0xF8) { // 65536 - 10FFFF
                ToType sixth      = (ToType)((*srcPtr) & 0x4) >> 2;
                ToType fifthHigh  = (ToType)((*srcPtr++) & 0x3);
                ToType fifthLow   = (ToType)((*srcPtr) & 0x30) >> 4;
                ToType fourth     = (ToType)((*srcPtr++) & 0xF);
                ToType third      = (ToType)((*srcPtr) & 0x3C) >> 2;
                ToType secondHigh = (ToType)((*srcPtr++) & 0x3);
                ToType secondLow  = (ToType)((*srcPtr) & 0x30) >> 4;
                ToType first      = (ToType)((*srcPtr++) & 0xF);

                int unicode = (sixth << 4) | (fifthHigh << 2) | fifthLow | (fourth << 12) | (third << 8) | (secondHigh << 6)
                              | (secondLow << 4) | first;
                ToType highSurrogate = (unicode - 0x10000) / 0x400 + 0xD800;
                ToType lowSurrogate  = (unicode - 0x10000) % 0x400 + 0xDC00;

                //Set the UTF-16 code units
                *dstPtr = lowSurrogate;
                dstPtr++;
                dstIterator++;

                //Check to see if we're still below the output string size before continuing, otherwise, we cut off here.
                if (dstIterator < srcLength) {
                    *dstPtr = highSurrogate;
                    dstPtr++;
                    dstIterator++;
                }

                //Increment the iterator to keep track of where we are in the UTF-8 string
                srcIterator += 4;
            } else {
                *dstPtr = (ToType)bogusSymbol;
                srcPtr++;
                dstPtr++;
            }
        }

        if (dst == nullptr) {
            dstPtr -= dstIterator;
            SAFE_DELETE_ARRAY(dstPtr);
            *dstLength = dstIterator + 1;
        } else {
            while (dstIterator < *dstLength) {
                *dstPtr = '\0';
                dstPtr++;
                dstIterator++;
            }
        }

        return dst;
    }
};