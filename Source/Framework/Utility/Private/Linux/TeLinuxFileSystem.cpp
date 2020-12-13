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

    void FileSystem::RemoveInternal(const String& path)
    {
        if (unix_isDirectory(path))
        {
            if (rmdir(path.c_str()))
                HANDLE_PATH_ERROR(path, errno);
        }
        else
        {
            if (unlink(path.c_str()))
                HANDLE_PATH_ERROR(path, errno);
        }
    }

    void FileSystem::CopyInternal(const String& source, const String& destination)
    {
        std::ifstream sourceStream(source.c_str(), std::ios::binary);
        std::ofstream destinationStream(destination.c_str(), std::ios::binary);

        destinationStream << sourceStream.rdbuf();
        sourceStream.close();
        destinationStream.close();
    }

    void FileSystem::MoveInternal(const String& oldPath, const String& newPath)
    {
        if (std::rename(oldPath.c_str(), newPath.c_str()) == -1)
        {
            // Cross-filesystem copy is likely needed (for example, /tmp to Banshee install dir while copying assets)
            std::ifstream src(oldPath.c_str(), std::ios::binary);
            std::ofstream dst(newPath.c_str(), std::ios::binary);
            dst << src.rdbuf(); // First, copy

            // Error handling
            src.close();
            if (!src)
            {
                TE_DEBUG(String(__FUNCTION__) + ": renaming " + oldPath + " to " + newPath + ": " + strerror(errno));
                return; // Do not remove source if we failed!
            }

            // Then, remove source file (hopefully succeeds)
            if (std::remove(oldPath.c_str()) == -1)
            {
                TE_DEBUG(String(__FUNCTION__) + ": renaming " + oldPath + " to " + newPath + ": " + strerror(errno));
            }
        }
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

    bool FileSystem::Exists(const String& path)
    {
        return unix_pathExists(path);
    }

    bool FileSystem::IsFile(const String& path)
    {
        return unix_pathExists(path) && unix_isFile(path);
    }

    bool FileSystem::IsDirectory(const String& path)
    {
        return unix_pathExists(path) && unix_isDirectory(path);
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
