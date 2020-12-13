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
         * Returns true if a file or a folder exists at the specified path.
         *
         * @param[in]	fullPath	Full path to a file or folder.
         */
        static bool Exists(const String& fullPath);

        /**
         * Returns true if a file exists at the specified path.
         *
         * @param[in]	fullPath	Full path to a file or folder.
         */
        static bool IsFile(const String& fullPath);

        /**
         * Returns true if a folder exists at the specified path.
         *
         * @param[in]	fullPath	Full path to a file or folder.
         */
        static bool IsDirectory(const String& fullPath);

        /**
         * Returns all files or folders located in the specified folder.
         *
         * @param[in]	dirPath			Full path to the folder to retrieve children files/folders from.
         * @param[out]	files	   		Pull paths to all files located directly in specified folder.
         * @param[out]	directories		Paths to all folders located directly in specified folder.
         * @param[in]	onlyFileName	If true, files and directories will return full paths, otherwise, only filenames
         */
        static void GetChildren(const String& dirPath, Vector<String>& files, Vector<String>& directories, bool onlyFileName = false);

        /** Returns the path to the currently working directory. */
        static String GetWorkingDirectoryPath();
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
