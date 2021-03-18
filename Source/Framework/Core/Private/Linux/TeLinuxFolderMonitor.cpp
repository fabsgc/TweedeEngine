#include "Platform/TeFolderMonitor.h"
#include "Utility/TeFileSystem.h"
#include <sys/inotify.h>

namespace te
{
    struct FolderMonitor::FolderWatchInfo
    {
        FolderWatchInfo(const String& folderToMonitor, int inHandle, bool monitorSubdirectories, UINT32 filter);
        ~FolderWatchInfo();

        void StartMonitor();
        void StopMonitor();

        void AddPath(const String& path);
        void RemovePath(const String& path);
        String GetPath(INT32 handle);

        String FolderToMonitor;
        int DirHandle;
        bool MonitorSubdirectories;
        UINT32 Filter;

        UnorderedMap<String, INT32> PathToHandle;
        UnorderedMap<INT32, String> HandleToPath;
    };

    FolderMonitor::FolderWatchInfo::FolderWatchInfo(const String& folderToMonitor, int inHandle, bool monitorSubdirectories, UINT32 filter)
        : FolderToMonitor(folderToMonitor)
        , DirHandle(inHandle)
        , MonitorSubdirectories(monitorSubdirectories)
        , Filter(filter)
    { }

    FolderMonitor::FolderWatchInfo::~FolderWatchInfo()
    {
        StopMonitor();
    }

    void FolderMonitor::FolderWatchInfo::StartMonitor()
    {
        AddPath(FolderToMonitor);

        if(MonitorSubdirectories)
        {
            FileSystem::Iterate(FolderToMonitor, nullptr, [this](const String& path)
            {
                AddPath(path);
                return true;
            });
        }
    }

    void FolderMonitor::FolderWatchInfo::StopMonitor()
    {
        for(auto& entry : PathToHandle)
            inotify_rm_watch(DirHandle, entry.second);

        PathToHandle.clear();
    }

    void FolderMonitor::FolderWatchInfo::AddPath(const String& path)
    {
        INT32 watchHandle = inotify_add_watch(DirHandle, path.c_str(), IN_ALL_EVENTS);
        if(watchHandle == -1)
        {
            String error = strerror(errno);
            TE_DEBUG("Unable to start folder monitor for path: \"" + path + "\". Error: " + error);
        }

        PathToHandle[path] = watchHandle;
        HandleToPath[watchHandle] = path;
    }

    void FolderMonitor::FolderWatchInfo::RemovePath(const String& path)
    {
        auto iterFind = PathToHandle.find(path);
        if(iterFind != PathToHandle.end())
        {
            INT32 watchHandle = iterFind->second;
            PathToHandle.erase(iterFind);

            HandleToPath.erase(watchHandle);
        }
    }

    String FolderMonitor::FolderWatchInfo::GetPath(INT32 handle)
    {
        auto iterFind = HandleToPath.find(handle);
        if(iterFind != HandleToPath.end())
            return iterFind->second;

        return String();
    }

    class FolderMonitor::FileNotifyInfo
    {
    };

    enum class FileActionType
    {
        Added,
        Removed,
        Modified,
        Renamed
    };

    struct FileAction
    {
        static FileAction* CreateAdded(const String& fileName)
        {
            UINT8* bytes = (UINT8*)te_allocate((UINT32)(sizeof(FileAction) + (fileName.size() + 1) * sizeof(String::value_type)));

            FileAction* action = (FileAction*)bytes;
            bytes += sizeof(FileAction);

            action->OldName = nullptr;
            action->NewName = (String::value_type*)bytes;
            action->Type = FileActionType::Added;

            memcpy(action->NewName, fileName.data(), fileName.size() * sizeof(String::value_type));
            action->NewName[fileName.size()] = L'\0';
            action->LastSize = 0;
            action->CheckForWriteStarted = false;

            return action;
        }

        static FileAction* CreateRemoved(const String& fileName)
        {
            UINT8* bytes = (UINT8*)te_allocate((UINT32)(sizeof(FileAction) + (fileName.size() + 1) * sizeof(String::value_type)));

            FileAction* action = (FileAction*)bytes;
            bytes += sizeof(FileAction);

            action->OldName = nullptr;
            action->NewName = (String::value_type*)bytes;
            action->Type = FileActionType::Removed;

            memcpy(action->NewName, fileName.data(), fileName.size() * sizeof(String::value_type));
            action->NewName[fileName.size()] = L'\0';
            action->LastSize = 0;
            action->CheckForWriteStarted = false;

            return action;
        }

        static FileAction* CreateModified(const String& fileName)
        {
            UINT8* bytes = (UINT8*)te_allocate((UINT32)(sizeof(FileAction) + (fileName.size() + 1) * sizeof(String::value_type)));

            FileAction* action = (FileAction*)bytes;
            bytes += sizeof(FileAction);

            action->OldName = nullptr;
            action->NewName = (String::value_type*)bytes;
            action->Type = FileActionType::Modified;

            memcpy(action->NewName, fileName.data(), fileName.size() * sizeof(String::value_type));
            action->NewName[fileName.size()] = L'\0';
            action->LastSize = 0;
            action->CheckForWriteStarted = false;

            return action;
        }

        static FileAction* CreateRenamed(const String& oldFilename, const String& newfileName)
        {
            UINT8* bytes = (UINT8*)te_allocate((UINT32)(sizeof(FileAction) +
                (oldFilename.size() + newfileName.size() + 2) * sizeof(String::value_type)));

            FileAction* action = (FileAction*)bytes;
            bytes += sizeof(FileAction);

            action->OldName = (String::value_type*)bytes;
            bytes += (oldFilename.size() + 1) * sizeof(String::value_type);

            action->NewName = (String::value_type*)bytes;
            action->Type = FileActionType::Modified;

            memcpy(action->OldName, oldFilename.data(), oldFilename.size() * sizeof(String::value_type));
            action->OldName[oldFilename.size()] = L'\0';

            memcpy(action->NewName, newfileName.data(), newfileName.size() * sizeof(String::value_type));
            action->NewName[newfileName.size()] = L'\0';
            action->LastSize = 0;
            action->CheckForWriteStarted = false;

            return action;
        }

        static void Destroy(FileAction* action)
        {
            te_free(action);
        }

        String::value_type* OldName;
        String::value_type* NewName;
        FileActionType Type;

        UINT64 LastSize;
        bool CheckForWriteStarted;
    };

    struct FolderMonitor::Pimpl
    {
        Vector<FolderWatchInfo*> Monitors;

        Vector<FileAction*> FileActions;
        Vector<FileAction*> ActiveFileActions;

        int InHandle;
        bool Started;
        Mutex MainMutex;
        Thread* WorkerThread;
    };

    FolderMonitor::FolderMonitor()
    {
        m = te_new<Pimpl>();
        m->WorkerThread = nullptr;
        m->InHandle = 0;
        m->Started = false;
    }

    FolderMonitor::~FolderMonitor()
    {
        StopMonitorAll();

        // No need for mutex since we know worker thread is shut down by now
        for(auto& action : m->FileActions)
            FileAction::Destroy(action);

        te_delete(m);
    }

    void FolderMonitor::StartMonitor(const String& folderPath, bool subdirectories, UINT32 changeFilter)
    { 
        if(!FileSystem::IsDirectory(folderPath))
        {
            TE_DEBUG("Provided path \"" + folderPath + "\" is not a directory");
            return;
        }

        // Check if there is overlap with existing monitors
        for(auto& monitor : m->Monitors)
        {
            // Identical monitor exists
            if(monitor->FolderToMonitor == folderPath)
            {
                TE_DEBUG("Folder is already monitored, cannot monitor it again.");
                return;
            }
        }

        // Initialize inotify if required
        if(!m->Started)
        {
            Lock lock(m->MainMutex);

            m->InHandle = inotify_init();
            m->Started = true;
        }

        FolderWatchInfo* watchInfo = te_new<FolderWatchInfo>(folderPath, m->InHandle, subdirectories, changeFilter);

        // Register and start the monitor
        {
            Lock lock(m->MainMutex);

            m->Monitors.push_back(watchInfo);
            watchInfo->StartMonitor();
        }

        // Start the worker thread if it isn't already
        if(m->WorkerThread == nullptr)
        {
            m->WorkerThread = te_new<Thread>(std::bind(&FolderMonitor::WorkerThreadMain, this));

            if(m->WorkerThread == nullptr)
                TE_DEBUG("Failed to create a new worker thread for folder monitoring");
        }
    }

    void FolderMonitor::StopMonitor(const String& folderPath)
    {
        auto findIter = std::find_if(m->Monitors.begin(), m->Monitors.end(),
            [&](const FolderWatchInfo* x) { return x->FolderToMonitor == folderPath; });

        if(findIter != m->Monitors.end())
        {
            // Special case if this is the last monitor
            if(m->Monitors.size() == 1)
                StopMonitorAll();
            else
            {
                Lock lock(m->MainMutex);
                FolderWatchInfo* watchInfo = *findIter;

                watchInfo->StopMonitor();
                te_delete(watchInfo);

                m->Monitors.erase(findIter);
            }
        }
    }

    void FolderMonitor::StopMonitorAll()
    {
        if(m->Started)
        {
            Lock lock(m->MainMutex);

            // First tell the thread it's ready to be shutdown
            m->Started = false;

            // Remove all watches (this will also wake up the thread). Note that at least one watch must be present otherwise
            // the thread won't wake up (we ensure that elsewhere).
            for (auto& watchInfo : m->Monitors)
            {
                watchInfo->StopMonitor();
                te_delete(watchInfo);
            }

            m->Monitors.clear();
        }

        // Wait for the thread to shutdown
        if(m->WorkerThread != nullptr)
        {
            m->WorkerThread->join();
            te_delete(m->WorkerThread);
            m->WorkerThread = nullptr;
        }

        // Close the inotify handle
        {
            Lock lock(m->MainMutex);
            if (m->InHandle != 0)
            {
                close(m->InHandle);
                m->InHandle = 0;
            }
        }
    }

    void FolderMonitor::WorkerThreadMain()
    {
        static const UINT32 BUFFER_SIZE = 16384;

        bool shouldRun;
        INT32 watchHandle;
        {
            Lock(m->MainMutex);
            watchHandle = m->InHandle;
            shouldRun = m->Started;
        }

        UINT8 buffer[BUFFER_SIZE];

        while(shouldRun)
        {
            INT32 length = (INT32)read(watchHandle, buffer, sizeof(buffer));

            // Handle was closed, shutdown thread
            if (length < 0)
                return;

            // Note: Must be after read, so shutdown can be started when we remove the watches (as then read() will return)
            {
                Lock(m->MainMutex);
                shouldRun = m->Started;
            }

            INT32 readPos = 0;
            while(readPos < length)
            {
                inotify_event* event = (inotify_event*)&buffer[readPos];
                if(event->len > 0)
                {
                    {
                        Lock lock(m->MainMutex);

                        String path;
                        FolderWatchInfo* monitor = nullptr;
                        for (auto& entry : m->Monitors)
                        {
                            entry->GetPath(event->wd);
                            if (!path.empty())
                            {
                                path += "/";
                                path += String(event->name);
                                monitor = entry;
                                break;
                            }
                        }

                        // This can happen if the path got removed during some recent previous event
                        if(monitor == nullptr)
                            goto next;

                        // Need to add/remove sub-directories to/from watch list
                        bool isDirectory = (event->mask & IN_ISDIR) != 0;
                        if(isDirectory && monitor->MonitorSubdirectories)
                        {
                            bool added = (event->mask & (IN_CREATE | IN_MOVED_TO)) != 0;
                            bool removed = (event->mask & (IN_DELETE | IN_MOVED_FROM)) != 0;

                            if(added)
                                monitor->AddPath(path);
                            else if(removed)
                                monitor->RemovePath(path);
                        }

                        // Actually trigger the events

                        // File/folder was added
                        if(((event->mask & (IN_CREATE | IN_MOVED_TO)) != 0))
                        {
                            if (isDirectory)
                            {
                                if (monitor->Filter & (UINT32)FolderChangeFlag::DirName)
                                    m->FileActions.push_back(FileAction::CreateAdded(path));
                            }
                            else
                            {
                                if (monitor->Filter & (UINT32)FolderChangeFlag::FileName)
                                    m->FileActions.push_back(FileAction::CreateAdded(path));
                            }
                        }

                        // File/folder was removed
                        if(((event->mask & (IN_DELETE | IN_MOVED_FROM)) != 0))
                        {
                            if(isDirectory)
                            {
                                if(monitor->Filter & (UINT32)FolderChangeFlag::DirName)
                                    m->FileActions.push_back(FileAction::CreateRemoved(path));
                            }
                            else
                            {
                                if(monitor->Filter & (UINT32)FolderChangeFlag::FileName)
                                    m->FileActions.push_back(FileAction::CreateRemoved(path));
                            }
                        }

                        // File was modified
                        if(((event->mask & IN_CLOSE_WRITE) != 0) && monitor->Filter & (UINT32)FolderChangeFlag::FileWrite)
                        {
                            m->FileActions.push_back(FileAction::CreateModified(path));
                        }

                        // Note: Not reporting renames, instead a remove + add event is created. To support renames I'd need
                        // to defer all event triggering until I have processed move event pairs and determined if the
                        // move is a rename (i.e. parent folder didn't change). All events need to be deferred (not just
                        // move events) in order to preserve the event ordering. For now this is too much hassle considering
                        // no external code relies on the rename functionality.
                    }
                }

                next:
                    readPos += sizeof(inotify_event) + event->len;
            }
        }
    }

    void FolderMonitor::HandleNotifications(FileNotifyInfo& notifyInfo, FolderWatchInfo& watchInfo)
    {
        // TODO
    }

    void FolderMonitor::Update()
    {
        {
            Lock lock(m->MainMutex);
            std::swap(m->FileActions, m->ActiveFileActions);
        }

        for(auto& action : m->ActiveFileActions)
        {
            switch (action->Type)
            {
            case FileActionType::Added:
                if (!OnAdded.Empty())
                    OnAdded(action->NewName);
                break;
            case FileActionType::Removed:
                if (!OnRemoved.Empty())
                    OnRemoved(action->NewName);
                break;
            case FileActionType::Modified:
                if (!OnModified.Empty())
                    OnModified(action->NewName);
                break;
            case FileActionType::Renamed:
                if (!OnRenamed.Empty())
                    OnRenamed(action->OldName, action->NewName);
                break;
            }

            FileAction::Destroy(action);
        }

        m->ActiveFileActions.clear();
    }
}
