#include "Utility/TeFileSystem.h"

namespace te
{
    void FileSystem::Copy(const String& oldPath, const String& newPath, bool overwriteExisting)
    {
        Stack<std::tuple<String, String>> todo;
        todo.push(std::make_tuple(oldPath, newPath));

        while (!todo.empty())
        {
            auto current = todo.top();
            todo.pop();

            String sourcePath = std::get<0>(current);
            if (!FileSystem::Exists(sourcePath))
                continue;

            bool srcIsFile = FileSystem::IsFile(sourcePath);
            String destinationPath = std::get<1>(current);
            bool destExists = FileSystem::Exists(destinationPath);

            if (destExists)
            {
                if (FileSystem::IsFile(destinationPath))
                {
                    if (overwriteExisting)
                        FileSystem::Remove(destinationPath);
                    else
                    {
                        TE_DEBUG("Copy operation failed because another file already exists at the new path: \"" + destinationPath + "\"");
                        return;
                    }
                }
            }

            if (srcIsFile)
            {
                FileSystem::CopyInternal(sourcePath, destinationPath);
            }
            else
            {
                // TODO create missing directories
                TE_DEBUG("Parent directory does not exist: \"" + destinationPath + "\"");
            }
        }
    }

    void FileSystem::Remove(const String& path, bool recursively)
    {
        if (!FileSystem::Exists(path))
            return;

        if (recursively)
        {
            Vector<String> files;
            Vector<String> directories;

            GetChildren(path, files, directories);

            for (auto& file : files)
                Remove(file, false);

            for (auto& dir : directories)
                Remove(dir, true);
        }

        FileSystem::RemoveInternal(path);
    }

    void FileSystem::Move(const String& oldPath, const String& newPath, bool overwriteExisting)
    {
        if (FileSystem::Exists(newPath))
        {
            if (overwriteExisting)
                FileSystem::Remove(newPath);
            else
            {
                TE_DEBUG("Move operation failed because another file already exists at the new path: \"" + newPath + "\"");
                return;
            }
        }

        FileSystem::MoveInternal(oldPath, newPath);
    }

    Mutex FileScheduler::_mutex;
}
