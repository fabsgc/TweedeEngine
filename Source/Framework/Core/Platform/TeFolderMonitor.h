#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeEvent.h"

namespace te
{
    /** Types of notifications we would like to receive when we start a FolderMonitor on a certain folder. */
    enum class FolderChangeFlag
    {
        FileName 	= 1 << 0, /**< Called when file is created, moved or removed. */
        DirName 	= 1 << 1, /**< Called when directory is created, moved or removed. */
        FileWrite 	= 1 << 2, /**< Called when file is written to. */
    };

    /**
     * Allows monitoring a file system folder for changes. Depending on the flags set this monitor can notify you when file
     * is changed/moved/renamed and similar.
     */
    class TE_CORE_EXPORT FolderMonitor
    {
        class FileNotifyInfo;

    public:
        struct Pimpl;
        struct FolderWatchInfo;

        FolderMonitor();
        ~FolderMonitor();

        /**
         * Starts monitoring a folder at the specified path.
         *
         * @param[in]	folderPath		Absolute path to the folder you want to monitor.
         * @param[in]	subdirectories	If true, provided folder and all of its subdirectories will be monitored for
         *								changes. Otherwise only the provided folder will be monitored.
         * @param[in]	changeFilter	A set of flags you may OR together. Different notification events will trigger
         *								depending on which flags you set.
         */
        void StartMonitor(const String& folderPath, bool subdirectories, UINT32 changeFilter);

        /** Stops monitoring the folder at the specified path. */
        void StopMonitor(const String& folderPath);

        /**	Stops monitoring all folders that are currently being monitored. */
        void StopMonitorAll();

        /** Triggers callbacks depending on events that ocurred. Expected to be called once per frame. */
        void Update();

        /** Triggers when a file in the monitored folder is modified. Provides absolute path to the file. */
        Event<void(const String&)> OnModified;

        /**	Triggers when a file/folder is added in the monitored folder. Provides absolute path to the file/folder. */
        Event<void(const String&)> OnAdded;

        /**	Triggers when a file/folder is removed from the monitored folder. Provides absolute path to the file/folder. */
        Event<void(const String&)> OnRemoved;

        /**	Triggers when a file/folder is renamed in the monitored folder. Provides absolute path with old and new names. */
        Event<void(const String&, const String&)> OnRenamed;

         /** Returns private data, for use by internal helper classes and methods. */
        Pimpl* GetPrivateData() const { return m; }

    private:
        /**	Worker method that monitors the IO ports for any modification notifications. */
        void WorkerThreadMain();

        /**	Called by the worker thread whenever a modification notification is received. */
        void HandleNotifications(FileNotifyInfo& notifyInfo, FolderWatchInfo& watchInfo);

        Pimpl* m;
    };
}
