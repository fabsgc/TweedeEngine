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
    };








    struct FolderMonitor::Pimpl
    { };
 
    FolderMonitor::FolderMonitor()
    {
        m = te_new<Pimpl>();
    }

    FolderMonitor::~FolderMonitor()
    {
        StopMonitorAll();

        te_delete(m);
    }

    void FolderMonitor::StartMonitor(const String& folderPath, bool subdirectories, UINT32 changeFilter)
    { }

    void FolderMonitor::StopMonitor(const String& folderPath)
    { }

    void FolderMonitor::StopMonitorAll()
    { }

    void FolderMonitor::WorkerThreadMain()
    { }

    void FolderMonitor::HandleNotifications(FileNotifyInfo& notifyInfo, FolderWatchInfo& watchInfo)
    { }

    void FolderMonitor::Update()
    { }
}
