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
            if (errno == ENOENT)    // No such file or directory
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

    bool FileSystem::Exists(const Path& path)
    {
        return unix_pathExists(path.toString());
    }

    bool FileSystem::IsFile(const Path& path)
    {
        String pathStr = path.toString();
        return unix_pathExists(pathStr) && unix_isFile(pathStr);
    }

    bool FileSystem::IsDirectory(const Path& path)
    {
        String pathStr = path.toString();
        return unix_pathExists(pathStr) && unix_isDirectory(pathStr);
    }
}
