#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"
#include "Threading/TeThreading.h"

namespace te
{
    /** Utility class for dealing with files. */
    class TE_UTILITY_EXPORT FileSystem
    {
    public:
        /**
         * Returns the size of a file in bytes.
         *
         * @param[in]	fullPath	Path to a file.
         */
        static UINT64 GetFileSize(const String& path);

        /**
         * Deletes a file or a folder at the specified path.
         *
         * @param[in]	fullPath   	Path to a file or a folder..
         * @param[in]	recursively	(optional) If true, folders will have their contents deleted as well. Otherwise an
         *							exception will be thrown for non-empty folders.
         */
        static bool Remove(const String& path, bool recursively = true);

        /**
         * Moves a file or a folder from one to another path. This can also be used as a rename operation.
         *
         * @param[in]	oldPath			 	Full path to the old file/folder.
         * @param[in]	newPath			 	Full path to the new file/folder.
         * @param[in]	overwriteExisting	(optional) If true, any existing file/folder at the new location will be
         *									overwritten, otherwise an exception will be thrown if a file/folder already exists.
         */
        static bool Move(const String& oldPath, const String& newPath, bool overwriteExisting = true);

        /**
         * Makes a copy of a file or a folder in the specified path.
         *
         * @param[in]	oldPath			 	Full path to the old file/folder.
         * @param[in]	newPath			 	Full path to the new file/folder.
         * @param[in]	overwriteExisting	(optional) If true, any existing file/folder at the new location will be
         *									overwritten, otherwise an exception will be thrown if a file/folder already exists.
         * @param[in]	recursively	(optional) If true, folders will have their contents copied as well.
         */
        static bool Copy(const String& oldPath, const String& newPath, bool overwriteExisting = true, bool recursively = true);

        /**
         * Creates a folder at the specified path.
         *
         * @param[in]	path	Path to a folder to create.
         */
        static bool CreateDir(const String& path);

        /**
         * Returns true if a file or a folder exists at the specified path.
         *
         * @param[in]	path	Path to a file or folder.
         */
        static bool Exists(const String& Path);

        /**
         * Returns true if a file exists at the specified path.
         *
         * @param[in]	path	Path to a file or folder.
         */
        static bool IsFile(const String& Path);

        /**
         * Returns true if a folder exists at the specified path.
         *
         * @param[in]	path	Path to a file or folder.
         */
        static bool IsDirectory(const String& Path);

        /**
         * Returns all files or folders located in the specified folder.
         *
         * @param[in]	dirPath			Full path to the folder to retrieve children files/folders from.
         * @param[out]	files	   		Pull paths to all files located directly in specified folder.
         * @param[out]	directories		Paths to all folders located directly in specified folder.
         * @param[in]	onlyFileName	If true, files and directories will return full paths, otherwise, only filenames
         */
        static void GetChildren(const String& dirPath, Vector<String>& files, Vector<String>& directories, bool onlyFileName = false);

        /**
         * Returns the last modified time of a file or a folder at the specified path.
         *
         * @param[in]	path	Path to a file or a folder.
         */
        static std::time_t GetLastModifiedTime(const String& path);

        /** Returns the path to the currently working directory. */
        static String GetWorkingDirectoryPath();

    private:
        /** Copy a single file. Internal function used by copy(). */
        static void CopyInternal(const String& oldPath, const String& newPath);
        /** Remove a single file. Internal function used by Remove(). */
        static void RemoveInternal(const String& path);
        /** Move a single file. Internal function used by Move(). */
        static void MoveInternal(const String& oldPath, const String& newPath);
    };

    /**
     * Locks access to files on the same drive, allowing only one file to be read at a time, per drive. This prevents
     * multiple threads accessing multiple files on the same drive at once, ruining performance on mechanical drives.
     */
    class TE_UTILITY_EXPORT FileScheduler final
    {
    public:
        /**
         * Locks access and doesn't allow other threads to get past this point until access is unlocked. Any scheduled
         * file access should happen past this point.
         */
        static void LockFile(const String& path)
        {
            // Note: File path should be analyzed and determined on which drive does the path belong to. Locks can then
            // be issued on a per-drive basis, instead of having one global lock. This would allow multiple files to be
            // accessed at the same time, as long as they're on different drives.
            _mutex.lock();
        }

        /**
         * Unlocks access and allows another thread to lock file access. Must be provided with the same file path as
         * lock().
         */
        static void UnlockFile(const String& path)
        {
            _mutex.unlock();
        }

        /**
         * Returns a lock object that immediately locks access (same as lock()), and then calls unlock() when it goes
         * out of scope.
         */
        static Lock GetLock(const String& path)
        {
            return Lock(_mutex);
        }

    private:
        static Mutex _mutex;
    };
}
