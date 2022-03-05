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

    void Util::Trim(String& str, bool left, bool right)
    {
        static const String delims = " \t\r";
        Trim(str, delims, left, right);
    }

    void Util::Trim(WString& str, bool left, bool right)
    {
        static const WString delims = L" \t\r";
        Trim(str, delims, left, right);
    }

    void Util::Trim(String& str, const String& delims, bool left, bool right)
    {
        if (right)
        {
            str.erase(str.find_last_not_of(delims) + 1); // trim right
        }
        if (left)
        {
            str.erase(0, str.find_first_not_of(delims)); // trim left
        }
    }

    void Util::Trim(WString& str, const WString& delims, bool left, bool right)
    {
        if (right)
        {
            str.erase(str.find_last_not_of(delims) + 1); // trim right
        }
        if (left)
        {
            str.erase(0, str.find_first_not_of(delims)); // trim left
        }
    }

    void Util::ToLowerCase(String& str)
    {
        te::ToLowerCase(str);
    }

    void Util::ToLowerCase(WString& str)
    {
        te::ToLowerCase(str);
    }

    void Util::ToUpperCase(String& str)
    {
        te::ToUpperCase(str);
    }

    void Util::ToUpperCase(WString& str)
    {
        te::ToUpperCase(str);
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
            TE_DEBUG("Can't find extension for file : " + filePath);
            return String();
        }
    }

    bool Util::IsBigEndian()
    {
        /*const int value{ 0x01 };
        const void* address = static_cast<const void*>(&value);
        const unsigned char* least_significant_address = static_cast<const unsigned char*>(address);
        return (*least_significant_address == 0x01);*/

        return TE_ENDIAN == TE_ENDIAN_BIG;
    }
}
