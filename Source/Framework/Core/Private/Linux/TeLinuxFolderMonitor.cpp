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
        // TODO

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

        // TODO
    }

    void FolderMonitor::StopMonitor(const String& folderPath)
    {
        // TODO
    }

    void FolderMonitor::StopMonitorAll()
    {
        // TODO
    }

    void FolderMonitor::WorkerThreadMain()
    {
        // TODO
    }

    void FolderMonitor::HandleNotifications(FileNotifyInfo& notifyInfo, FolderWatchInfo& watchInfo)
    {
        // TODO
    }

    void FolderMonitor::Update()
    {
        // TODO
    }
}
