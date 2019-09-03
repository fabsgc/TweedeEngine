#include "Prerequisites/TePrerequisitesUtility.h"
#include "ThirdParty/Md5/md5.h"

namespace te
{
    Vector<String> Util::Split(const String& str, const String& delims, UINT16 maxSplits)
    {
        return SplitInternal<char>(str, delims, maxSplits);
    }

    Vector<WString> Util::Split(const WString& str, const WString& delims, UINT16 maxSplits)
    {
        return SplitInternal<wchar_t>(str, delims, maxSplits);
    }

    void Util::ToLowerCase(String& str)
    {
        std::transform(str.begin(), str.end(), str.begin(), tolower);
    }

    void Util::ToLowerCase(WString& str)
    {
        std::transform(str.begin(), str.end(), str.begin(), tolower);
    }

    void Util::ToUpperCase(String& str)
    {
        std::transform(str.begin(), str.end(), str.begin(), toupper);
    }

    void Util::ToUpperCase(WString& str)
    {
        std::transform(str.begin(), str.end(), str.begin(), toupper);
    }

    String Util::Md5(const WString& source)
    {
        MD5 md5;
        md5.update((UINT8*)source.data(), (UINT32)source.length() * sizeof(WString::value_type));
        md5.finalize();

        UINT8 digest[16];
        md5.decdigest(digest, sizeof(digest));

        String buf;
        buf.resize(32);
        for (UINT8 i = 0; i < 16; i++)
        {
            auto f = (UINT64)i * 2;
            snprintf(&(buf[0]) + f, 3, "%02x", digest[i]);
        }

        return buf;
    }

    String Util::Md5(const String& source)
    {
        MD5 md5;
        md5.update((UINT8*)source.data(), (UINT32)source.length() * sizeof(String::value_type));
        md5.finalize();

        UINT8 digest[16];
        md5.decdigest(digest, sizeof(digest));

        String buf;
        buf.resize(32);
        for (int i = 0; i < 16; i++)
        {
            auto f = (UINT64)i * 2;
            snprintf(&(buf[0]) + f, 3, "%02x", digest[i]);
        }

        return buf;
    }

    void Util::GetTime(char* buffer)
    {
        time_t rawtime;
        struct tm * timeinfo;

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
    }

    String Util::GetFileExtension(const String& filePath)
    {
        String::size_type pos = filePath.rfind('.');
        if (pos != String::npos)
        {
            return filePath.substr(pos);
        }
        else
        {
            TE_DEBUG("Can't find extension for file : " + filePath, __FILE__, __LINE__);
            return String();
        }
    }
}