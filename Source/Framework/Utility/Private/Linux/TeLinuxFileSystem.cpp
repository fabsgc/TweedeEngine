#include "Utility/TeFileSystem.h"

#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <climits>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <fstream>

#define HANDLE_PATH_ERROR(path__, errno__) \
    TE_DEBUG((String(__FUNCTION__) + ": " + String(path__) + ": " + String(strerror(errno__))));

namespace te
{
    bool unix_pathExists(const String& path)
    {
        struct stat st_buf;
        if (stat(path.c_str(), &st_buf) == 0)
            return true;
        else
            if (errno == ENOENT) // No such file or directory
                return false;
            else
            {
                HANDLE_PATH_ERROR(path, errno);
                return false;
            }
    }

    bool unix_stat(const String& path, struct stat *st_buf)
    {
        if (stat(path.c_str(), st_buf) != 0)
        {
            HANDLE_PATH_ERROR(path, errno);
            return false;
        }
        return true;
    }

    bool unix_isFile(const String& path)
    {
        struct stat st_buf;
        if (unix_stat(path, &st_buf))
            return S_ISREG(st_buf.st_mode);

        return false;
    }

    bool unix_isDirectory(const String& path)
    {
        struct stat st_buf;
        if (unix_stat(path, &st_buf))
            return S_ISDIR(st_buf.st_mode);

        return false;
    }

    UINT64 FileSystem::GetFileSize(const String& path)
    {
        struct stat st_buf;

        if (stat(path.c_str(), &st_buf) == 0)
        {
            return (UINT64)st_buf.st_size;
        }
        else
        {
            HANDLE_PATH_ERROR(path, errno);
            return (UINT64)-1;
        }
    }

    void FileSystem::GetChildren(const String& dirPath, Vector<String>& files, Vector<String>& directories, bool onlyFileName)
    {
        const String pathStr = dirPath;

        if (unix_isFile(pathStr))
            return;

        DIR *dp = opendir(pathStr.c_str());
        if (dp == NULL)
        {
            HANDLE_PATH_ERROR(pathStr, errno);
            return;
        }

        struct dirent *ep;
        while ( (ep = readdir(dp)) )
        {
            const String filename(ep->d_name);
            if (filename != "." && filename != "..")
            {
                if (unix_isDirectory(pathStr + "/" + filename))
                    directories.push_back(dirPath + (filename + "/"));
                else
                    files.push_back(dirPath + filename);
            }
        }
        closedir(dp);
    }

    std::time_t FileSystem::GetLastModifiedTime(const String& path)
    {
        struct stat st_buf;
        stat(path.c_str(), &st_buf);
        std::time_t time = st_buf.st_mtime;

        return time;
    }

    String FileSystem::GetWorkingDirectoryPath()
    {
        char *buffer = te_newN<char>(PATH_MAX);

        String wd;
        if (getcwd(buffer, PATH_MAX) != nullptr)
            wd = buffer;
        else
            TE_DEBUG("Error when calling getcwd(): " + String(strerror(errno)));

        te_free(buffer);
        return wd;
    }
}
