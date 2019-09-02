#include "Prerequisites/TePrerequisitesUtility.h"
#include "ThirdParty/Md5/md5.h"

namespace te
{
    String Md5(const WString& source)
    {
        MD5 md5;
        md5.update((UINT8*)source.data(), (UINT32)source.length() * sizeof(WString::value_type));
        md5.finalize();

        UINT8 digest[16];
        md5.decdigest(digest, sizeof(digest));

        String buf;
        buf.resize(32);
        for (int i = 0; i < 16; i++)
        {
            snprintf(&(buf[0]) + i * 2, 3, "%02x", digest[i]);
        }

        return buf;
    }

    String Md5(const String& source)
    {
        MD5 md5;
        md5.update((UINT8*)source.data(), (UINT32)source.length() * sizeof(String::value_type));
        md5.finalize();

        UINT8 digest[16];
        md5.decdigest(digest, sizeof(digest));

        String buf;
        buf.resize(32);
        for (int i = 0; i < 16; i++)
            snprintf(&(buf[0]) + i * 2, 3, "%02x", digest[i]);

        return buf;
    }

    void GetTime(char* buffer)
    {
        time_t rawtime;
        struct tm * timeinfo;

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
    }

    String GetFileExtension(const String& filePath)
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