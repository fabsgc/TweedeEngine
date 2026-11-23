#include "Prerequisites/TePrerequisitesUtility.h"
#include "String/TeString.h"

#include <cctype>
#include <limits>

#if TE_PLATFORM == TE_PLATFORM_LINUX
#include <cerrno>
#include <iconv.h>
#endif

namespace te
{
    WString ToWString(const String& source)
    {
        return WString(source.begin(), source.end());
    }

    WString ToWString(const char* source)
    {
        return ToWString(String(source));
    }

    WString ToWString(float val, unsigned short precision,
        unsigned short width, char fill, std::ios::fmtflags flags)
    {
        WStringStream stream;
        stream.precision(precision);
        stream.width(width);
        stream.fill(fill);
        if (flags)
            stream.setf(flags);
        stream << val;
        return stream.str();
    }

    WString ToWString(double val, unsigned short precision,
        unsigned short width, char fill, std::ios::fmtflags flags)
    {
        WStringStream stream;
        stream.precision(precision);
        stream.width(width);
        stream.fill(fill);
        if (flags)
            stream.setf(flags);
        stream << val;
        return stream.str();
    }

    WString ToWString(int val,
        unsigned short width, char fill, std::ios::fmtflags flags)
    {
        WStringStream stream;
        stream.width(width);
        stream.fill(fill);
        if (flags)
            stream.setf(flags);
        stream << val;
        return stream.str();
    }

    WString ToWString(unsigned int val, unsigned short width, char fill, std::ios::fmtflags flags)
    {
        WStringStream stream;
        stream.width(width);
        stream.fill(fill);
        if (flags)
            stream.setf(flags);
        stream << val;
        return stream.str();
    }

    WString ToWString(INT64 val, unsigned short width, char fill, std::ios::fmtflags flags)
    {
        WStringStream stream;
        stream.width(width);
        stream.fill(fill);
        if (flags)
            stream.setf(flags);
        stream << val;
        return stream.str();
    }

    WString ToWString(UINT64 val, unsigned short width, char fill, std::ios::fmtflags flags)
    {
        WStringStream stream;
        stream.width(width);
        stream.fill(fill);
        if (flags)
            stream.setf(flags);
        stream << val;
        return stream.str();
    }

    WString ToWString(char val, unsigned short width, char fill, std::ios::fmtflags flags)
    {
        WStringStream stream;
        stream.width(width);
        stream.fill(fill);
        if (flags)
            stream.setf(flags);
        stream << val;
        return stream.str();
    }

    WString ToWString(wchar_t val, unsigned short width, char fill, std::ios::fmtflags flags)
    {
        WStringStream stream;
        stream.width(width);
        stream.fill(fill);
        if (flags)
            stream.setf(flags);
        stream << val;
        return stream.str();
    }

    String ToString(const WString& source)
    {
        if (source.empty())
            return String();

#if TE_PLATFORM == TE_PLATFORM_WIN32
        if (source.size() > static_cast<size_t>((std::numeric_limits<int>::max)()))
        {
            TE_ASSERT_ERROR(false, "Wide string is too long to convert to UTF-8");
            return String();
        }

        const int sourceLength = static_cast<int>(source.size());
        const int outputLength = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, source.c_str(), sourceLength, nullptr, 0, nullptr, nullptr);
        if (outputLength == 0)
        {
            TE_ASSERT_ERROR(false, "WideCharToMultiByte failed to size UTF-8 output");
            return String();
        }

        String result(static_cast<size_t>(outputLength), '\0');
        const int convertedLength = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, source.c_str(), sourceLength, &result[0], outputLength, nullptr, nullptr);
        if (convertedLength == 0)
        {
            TE_ASSERT_ERROR(false, "WideCharToMultiByte failed to convert the wide string");
            return String();
        }

        result.resize(static_cast<size_t>(convertedLength));
        return result;
#elif TE_PLATFORM == TE_PLATFORM_LINUX
        if (source.size() > (std::numeric_limits<size_t>::max)() / sizeof(wchar_t))
        {
            TE_ASSERT_ERROR(false, "Wide string is too long to convert to UTF-8");
            return String();
        }

        String result;
        if (source.size() > result.max_size() / 4)
        {
            TE_ASSERT_ERROR(false, "Wide string is too long to convert to UTF-8");
            return String();
        }

        iconv_t converter = iconv_open("UTF-8", "WCHAR_T");
        if (converter == reinterpret_cast<iconv_t>(-1))
        {
            TE_ASSERT_ERROR(false, "iconv_open failed to initialize UTF-8 conversion");
            return String();
        }

        struct IconvCloser
        {
            explicit IconvCloser(iconv_t descriptor) : descriptor(descriptor) {}
            ~IconvCloser() { iconv_close(descriptor); }
            iconv_t descriptor;
        } closer(converter);

        const size_t inputSize = source.size() * sizeof(wchar_t);
        char* input = reinterpret_cast<char*>(const_cast<wchar_t*>(source.data()));
        size_t inputBytesLeft = inputSize;

        result.resize(source.size() * 4);
        char* output = &result[0];
        size_t outputBytesLeft = result.size();

        while (iconv(converter, &input, &inputBytesLeft, &output, &outputBytesLeft)
            == static_cast<size_t>(-1))
        {
            if (errno != E2BIG)
            {
                TE_ASSERT_ERROR(false, "iconv failed to convert the wide string to UTF-8");
                return String();
            }

            const size_t bytesWritten = static_cast<size_t>(output - &result[0]);
            const size_t additionalCapacity = result.size() > 16 ? result.size() : 16;
            if (result.size() > result.max_size() - additionalCapacity)
            {
                TE_ASSERT_ERROR(false, "Wide string is too long to convert to UTF-8");
                return String();
            }

            result.resize(result.size() + additionalCapacity);
            output = &result[0] + bytesWritten;
            outputBytesLeft = result.size() - bytesWritten;
        }

        if (inputBytesLeft != 0)
        {
            TE_ASSERT_ERROR(false, "iconv did not convert the complete wide string");
            return String();
        }

        result.resize(static_cast<size_t>(output - &result[0]));
        return result;
#else
        TE_ASSERT_ERROR(false, "Wide string to UTF-8 conversion is unsupported on this platform");
        return String();
#endif
    }

    String ToString(const wchar_t* source)
    {
        return ToString(WString(source));
    }

    String ToString(float val, unsigned short precision,
        unsigned short width, char fill, std::ios::fmtflags flags)
    {
        StringStream stream;
        stream.precision(precision);
        stream.width(width);
        stream.fill(fill);
        if (flags)
            stream.setf(flags);
        stream << val;
        return stream.str();
    }

    String ToString(double val, unsigned short precision,
        unsigned short width, char fill, std::ios::fmtflags flags)
    {
        StringStream stream;
        stream.precision(precision);
        stream.width(width);
        stream.fill(fill);
        if (flags)
            stream.setf(flags);
        stream << val;
        return stream.str();
    }

    String ToString(int val,
        unsigned short width, char fill, std::ios::fmtflags flags)
    {
        StringStream stream;
        stream.width(width);
        stream.fill(fill);
        if (flags)
            stream.setf(flags);
        stream << val;
        return stream.str();
    }

    String ToString(unsigned int val, unsigned short width, char fill, std::ios::fmtflags flags)
    {
        StringStream stream;
        stream.width(width);
        stream.fill(fill);
        if (flags)
            stream.setf(flags);
        stream << val;
        return stream.str();
    }

    String ToString(INT64 val,
        unsigned short width, char fill, std::ios::fmtflags flags)
    {
        StringStream stream;
        stream.width(width);
        stream.fill(fill);
        if (flags)
            stream.setf(flags);
        stream << val;
        return stream.str();
    }

    String ToString(UINT64 val, unsigned short width, char fill, std::ios::fmtflags flags)
    {
        StringStream stream;
        stream.width(width);
        stream.fill(fill);
        if (flags)
            stream.setf(flags);
        stream << val;
        return stream.str();
    }

    float ParseFloat(const String& val, float defaultValue)
    {
        // Use istringstream for direct correspondence with toString
        StringStream str(val);
        float ret = defaultValue;
        str >> ret;

        return ret;
    }

    INT32 ParseINT32(const String& val, INT32 defaultValue)
    {
        // Use istringstream for direct correspondence with toString
        StringStream str(val);
        INT32 ret = defaultValue;
        str >> ret;

        return ret;
    }

    UINT32 ParseUINT32(const String& val, UINT32 defaultValue)
    {
        // Use istringstream for direct correspondence with toString
        StringStream str(val);
        UINT32 ret = defaultValue;
        str >> ret;

        return ret;
    }

    INT64 ParseINT64(const String& val, INT64 defaultValue)
    {
        // Use istringstream for direct correspondence with toString
        StringStream str(val);
        INT64 ret = defaultValue;
        str >> ret;

        return ret;
    }

    UINT64 ParseUINT64(const String& val, UINT64 defaultValue)
    {
        // Use istringstream for direct correspondence with toString
        StringStream str(val);
        UINT64 ret = defaultValue;
        str >> ret;

        return ret;
    }

    bool IsNumber(const String& val)
    {
        StringStream str(val);
        float tst;
        str >> tst;
        return !str.fail() && str.eof();
    }

    float ParseFloat(const WString& val, float defaultValue)
    {
        // Use istringstream for direct correspondence with toString
        WStringStream str(val);
        float ret = defaultValue;
        str >> ret;

        return ret;
    }

    INT32 ParseINT32(const WString& val, INT32 defaultValue)
    {
        // Use istringstream for direct correspondence with toString
        WStringStream str(val);
        INT32 ret = defaultValue;
        str >> ret;

        return ret;
    }

    UINT32 ParseUINT32(const WString& val, UINT32 defaultValue)
    {
        // Use istringstream for direct correspondence with toString
        WStringStream str(val);
        UINT32 ret = defaultValue;
        str >> ret;

        return ret;
    }

    INT64 ParseINT64(const WString& val, INT64 defaultValue)
    {
        // Use istringstream for direct correspondence with toString
        WStringStream str(val);
        INT64 ret = defaultValue;
        str >> ret;

        return ret;
    }

    UINT64 ParseUINT64(const WString& val, UINT64 defaultValue)
    {
        // Use istringstream for direct correspondence with toString
        WStringStream str(val);
        UINT64 ret = defaultValue;
        str >> ret;

        return ret;
    }

    bool IsNumber(const WString& val)
    {
        WStringStream str(val);
        float tst;
        str >> tst;
        return !str.fail() && str.eof();
    }

    Vector<String> Split(const String& s, char delimiter)
    {
        Vector<String> tokens;
        String token;
        IStringStream tokenStream(s);

        while (std::getline(tokenStream, token, delimiter))
        {
            tokens.push_back(token);
        }

        return tokens;
    }

    Vector<String> Split(const WString& s, char delimiter)
    {
        Vector<String> tokens;
        String str = ToString(s);
        String token;
        IStringStream tokenStream(str);

        while (std::getline(tokenStream, token, delimiter))
        {
            tokens.push_back(token);
        }

        return tokens;
    }

    void ToLowerCase(String& str)
    {
        std::transform(str.begin(), str.end(), str.begin(),
            [](unsigned char c) -> unsigned char { return static_cast<unsigned char>(std::tolower(c)); });
    }

    void ToLowerCase(WString& str)
    {
        std::transform(str.begin(), str.end(), str.begin(),
            [](wchar_t c) -> wchar_t { return static_cast<wchar_t>(std::tolower(c)); });
    }

    void ToUpperCase(String& str)
    {
        std::transform(str.begin(), str.end(), str.begin(),
            [](unsigned char c) -> unsigned char { return static_cast<unsigned char>(std::toupper(c)); });
    }

    void ToUpperCase(WString& str)
    {
        std::transform(str.begin(), str.end(), str.begin(),
            [](wchar_t c) -> wchar_t { return static_cast<wchar_t>(std::toupper(c)); });
    }

    bool StartsWith(const String& str, const String& pattern, bool lowerCase)
    {
        return StartsWithInternal<char>(str, pattern, lowerCase);
    }

    bool StartsWith(const WString& str, const WString& pattern, bool lowerCase)
    {
        return StartsWithInternal<wchar_t>(str, pattern, lowerCase);
    }

    bool EndsWith(const String& str, const String& pattern, bool lowerCase)
    {
        return EndsWithInternal<char>(str, pattern, lowerCase);
    }

    bool EndsWith(const WString& str, const WString& pattern, bool lowerCase)
    {
        return EndsWithInternal<wchar_t>(str, pattern, lowerCase);
    }

    bool Match(const String& str, const String& pattern, bool caseSensitive)
    {
        return MatchInternal<char>(str, pattern, caseSensitive);
    }

    bool Match(const WString& str, const WString& pattern, bool caseSensitive)
    {
        return MatchInternal<wchar_t>(str, pattern, caseSensitive);
    }

    bool RegexMatch(const String& str, const String& pattern, bool caseSensitive)
    {
        return RegexMatchInternal<char>(str, pattern, caseSensitive);
    }

    bool RegexMatch(const WString& str, const WString& pattern, bool caseSensitive)
    {
        return RegexMatchInternal<wchar_t>(str, pattern, caseSensitive);
    }

    const String ReplaceAll(const String& source, const String& replaceWhat, const String& replaceWithWhat)
    {
        return ReplaceAllInternal<char>(source, replaceWhat, replaceWithWhat);
    }

    const WString ReplaceAll(const WString& source, const WString& replaceWhat, const WString& replaceWithWhat)
    {
        return ReplaceAllInternal<wchar_t>(source, replaceWhat, replaceWithWhat);
    }
}
