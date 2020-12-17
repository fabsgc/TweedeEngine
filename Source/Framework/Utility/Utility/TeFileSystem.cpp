#include "Utility/TeFileSystem.h"
#include <filesystem>

namespace te
{
    bool FileSystem::Copy(const String& oldPath, const String& newPath, bool overwriteExisting, bool recursively)
    {
        std::filesystem::copy_options options;

        if (std::filesystem::exists(newPath) && !overwriteExisting)
        {
            TE_DEBUG("Copy operation failed because another file already exists at the new path: \"" + newPath + "\"");
            return false;
        }

        if (overwriteExisting)
            options |= std::filesystem::copy_options::overwrite_existing;
        if (recursively)
            options |= std::filesystem::copy_options::recursive;

        std::filesystem::copy(oldPath, newPath, options);
        return true;
    }

    bool FileSystem::Remove(const String& path, bool recursively)
    {
        if (!std::filesystem::exists(path))
            return false;

        if (recursively)
            return std::filesystem::remove_all(path);
        else
            return std::filesystem::remove(path);
    }

    bool FileSystem::Move(const String& oldPath, const String& newPath, bool overwriteExisting)
    {
        if (std::filesystem::exists(newPath) && !overwriteExisting)
        {
            TE_DEBUG("Move operation failed because another file already exists at the new path: \"" + newPath + "\"");
            return false;
        }

        std::filesystem::rename(oldPath, newPath);
        if (std::filesystem::exists(newPath))
            return true;

        return false;
    }

    bool FileSystem::CreateDir(const String& path)
    {
        if(std::filesystem::is_directory(path) && !std::filesystem::exists(path))
            return std::filesystem::create_directories(path);

        if(std::filesystem::exists(path))
            return true;

        return false;
    }

    bool FileSystem::Exists(const String& path)
    {
        return std::filesystem::exists(path);
    }

    bool FileSystem::IsFile(const String& path)
    {
        return std::filesystem::is_regular_file(path);
    }

    bool FileSystem::IsDirectory(const String& path)
    {
        return std::filesystem::is_directory(path);
    }

    Mutex FileScheduler::_mutex;
}
