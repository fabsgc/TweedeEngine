#include "Platform/TeFolderMonitor.h"
#include "Utility/TeFileSystem.h"
#include <sys/inotify.h>

namespace te
{
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
