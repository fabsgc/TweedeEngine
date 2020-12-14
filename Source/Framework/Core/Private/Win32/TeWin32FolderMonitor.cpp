#include "Platform/TeFolderMonitor.h"
#include "Utility/TeFileSystem.h"

#include <windows.h>
#include "String/TeUnicode.h"

namespace te
{ 
    enum class MonitorState
    {
        Inactive,
        Starting,
        Monitoring,
        Shutdown,
        Shutdown2
    };

    class WorkerFunc
    {
    public:
        WorkerFunc(FolderMonitor* owner);

        void operator()();

    private:
        FolderMonitor* _owner;
    };

    struct FolderMonitor::FolderWatchInfo
    {
        FolderWatchInfo(const String& folderToMonitor, HANDLE dirHandle, bool monitorSubdirectories, DWORD monitorFlags);
        ~FolderWatchInfo();

        void StartMonitor(HANDLE compPortHandle);
        void StopMonitor(HANDLE compPortHandle);

        static const UINT32 READ_BUFFER_SIZE = 65536;

        String FolderToMonitor;
        HANDLE DirHandle;
        OVERLAPPED Overlapped;
        MonitorState State;
        UINT8 Buffer[READ_BUFFER_SIZE];
        DWORD BufferSize;
        bool MonitorSubdirectories;
        DWORD MonitorFlags;
        DWORD ReadError;

        WString CachedOldFileName; // Used during rename notifications as they are handled in two steps

        Mutex StatusMutex;
        Signal StartStopEvent;
    };

    FolderMonitor::FolderWatchInfo::FolderWatchInfo(const String& folderToMonitor, HANDLE dirHandle, bool monitorSubdirectories, DWORD monitorFlags)
        : FolderToMonitor(folderToMonitor)
        , DirHandle(dirHandle)
        , State(MonitorState::Inactive)
        , BufferSize(0)
        , MonitorSubdirectories(monitorSubdirectories)
        , MonitorFlags(monitorFlags)
        , ReadError(0)
    {
        memset(&Overlapped, 0, sizeof(Overlapped));
    }

    void FolderMonitor::FolderWatchInfo::StartMonitor(HANDLE compPortHandle)
    { }

    FolderMonitor::FolderWatchInfo::~FolderWatchInfo()
    {
        assert(State == MonitorState::Inactive);

        StopMonitor(0);
    }

    void FolderMonitor::FolderWatchInfo::StopMonitor(HANDLE compPortHandle)
    { 
        // TODO
    }

    class FolderMonitor::FileNotifyInfo
    {
        // TODO
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
    { 
        Vector<FolderWatchInfo*> FoldersToWatch;
        HANDLE CompPortHandle;

        Queue<FileAction*> FileActions;
        List<FileAction*> ActiveFileActions;

        Mutex MainMutex;
        Thread* WorkerThread;
    };

    FolderMonitor::FolderMonitor()
    {
        m = te_new<Pimpl>();
        m->WorkerThread = nullptr;
        m->CompPortHandle = nullptr;
    }

    FolderMonitor::~FolderMonitor()
    {
        StopMonitorAll();

        // TODO

        te_delete(m);
    }

    void FolderMonitor::StartMonitor(const String& folderPath, bool subdirectories, UINT32 changeFilter)
    { 
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
