#include "Platform/TeFolderMonitor.h"
#include "Utility/TeFileSystem.h"

#include <windows.h>
#include <filesystem>
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

    String BuildWindowsPath(const String& path)
    {
        return ReplaceAll(path, "/", "\\");
    }

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

    FolderMonitor::FolderWatchInfo::~FolderWatchInfo()
    {
        assert(State == MonitorState::Inactive);

        StopMonitor(0);
    }

    void FolderMonitor::FolderWatchInfo::StartMonitor(HANDLE compPortHandle)
    { 
        if (State != MonitorState::Inactive)
            return; // Already monitoring

        {
            Lock lock(StatusMutex);

            State = MonitorState::Starting;
            PostQueuedCompletionStatus(compPortHandle, sizeof(this), (ULONG_PTR)this, &Overlapped);

            while (State != MonitorState::Monitoring)
                StartStopEvent.wait(lock);
        }

        if (ReadError != ERROR_SUCCESS)
        {
            {
                Lock lock(StatusMutex);
                State = MonitorState::Inactive;
            }

            TE_ASSERT_ERROR(false, "Failed to start folder monitor on folder \"" + FolderToMonitor + "\" because ReadDirectoryChangesW failed.");
        }
    }

    void FolderMonitor::FolderWatchInfo::StopMonitor(HANDLE compPortHandle)
    { 
        if (State != MonitorState::Inactive)
        {
            Lock lock(StatusMutex);

            State = MonitorState::Shutdown;
            PostQueuedCompletionStatus(compPortHandle, sizeof(this), (ULONG_PTR)this, &Overlapped);

            while (State != MonitorState::Inactive)
                StartStopEvent.wait(lock);
        }

        if (DirHandle != INVALID_HANDLE_VALUE)
        {
            CloseHandle(DirHandle);
            DirHandle = INVALID_HANDLE_VALUE;
        }
    }

    class FolderMonitor::FileNotifyInfo
    {
    public:
        FileNotifyInfo(UINT8* notifyBuffer, DWORD bufferSize)
            : _buffer(notifyBuffer)
            , _bufferSize(bufferSize)
        {
            _currentRecord = (PFILE_NOTIFY_INFORMATION)_buffer;
        }

        bool GetNext();

        DWORD	GetAction() const;
        WString GetFileName() const;
        WString GetFileNameWithPath(const String& rootPath) const;

    protected:
        UINT8* _buffer;
        DWORD _bufferSize;
        PFILE_NOTIFY_INFORMATION _currentRecord;
    };

    bool FolderMonitor::FileNotifyInfo::GetNext()
    {
        if (_currentRecord && _currentRecord->NextEntryOffset != 0)
        {
            PFILE_NOTIFY_INFORMATION oldRecord = _currentRecord;
            _currentRecord = (PFILE_NOTIFY_INFORMATION)((UINT8*)_currentRecord + _currentRecord->NextEntryOffset);

            if ((DWORD)((UINT8*)_currentRecord - _buffer) > _bufferSize)
            {
                // Gone out of range, something bad happened
                assert(false);

                _currentRecord = oldRecord;
            }

            return (_currentRecord != oldRecord);
        }

        return false;
    }

    DWORD FolderMonitor::FileNotifyInfo::GetAction() const
    {
        assert(_currentRecord != nullptr);

        if (_currentRecord)
            return _currentRecord->Action;

        return 0;
    }

    WString FolderMonitor::FileNotifyInfo::GetFileName() const
    {
        if (_currentRecord)
        {
            wchar_t fileNameBuffer[32768 + 1] = { 0 };

            memcpy(fileNameBuffer, _currentRecord->FileName,
                std::min(DWORD(32768 * sizeof(wchar_t)), _currentRecord->FileNameLength));

            return WString(fileNameBuffer);
        }

        return WString();
    }

    WString FolderMonitor::FileNotifyInfo::GetFileNameWithPath(const String& rootPath) const
    {
        String fullPath = rootPath;
        String fileName = UTF8::FromWide(GetFileName());
        fullPath.append(fileName);

        return UTF8::ToWide(fullPath);
    }

    enum class FileActionType
    {
        Added,
        Removed,
        Modified,
        Renamed
    };

    struct FileAction
    {
        static FileAction* CreateAdded(const WString& fileName)
        {
            String utf8filename = UTF8::FromWide(fileName);
            UINT8* bytes = (UINT8*)te_allocate((UINT32)(sizeof(FileAction) + (utf8filename.size() + 1) * sizeof(String::value_type)));

            FileAction* action = (FileAction*)bytes;
            bytes += sizeof(FileAction);

            action->OldName = nullptr;
            action->NewName = (String::value_type*)bytes;
            action->Type = FileActionType::Added;

            memcpy(action->NewName, utf8filename.data(), utf8filename.size() * sizeof(String::value_type));
            action->NewName[utf8filename.size()] = L'\0';
            action->LastSize = 0;
            action->CheckForWriteStarted = false;

            return action;
        }

        static FileAction* CreateRemoved(const WString& fileName)
        {
            String utf8filename = UTF8::FromWide(fileName);
            UINT8* bytes = (UINT8*)te_allocate((UINT32)(sizeof(FileAction) + (utf8filename.size() + 1) * sizeof(String::value_type)));

            FileAction* action = (FileAction*)bytes;
            bytes += sizeof(FileAction);

            action->OldName = nullptr;
            action->NewName = (String::value_type*)bytes;
            action->Type = FileActionType::Removed;

            memcpy(action->NewName, utf8filename.data(), utf8filename.size() * sizeof(String::value_type));
            action->NewName[utf8filename.size()] = L'\0';
            action->LastSize = 0;
            action->CheckForWriteStarted = false;

            return action;
        }

        static FileAction* CreateModified(const WString& fileName)
        {
            String utf8filename = UTF8::FromWide(fileName);
            UINT8* bytes = (UINT8*)te_allocate((UINT32)(sizeof(FileAction) + (utf8filename.size() + 1) * sizeof(String::value_type)));

            FileAction* action = (FileAction*)bytes;
            bytes += sizeof(FileAction);

            action->OldName = nullptr;
            action->NewName = (String::value_type*)bytes;
            action->Type = FileActionType::Modified;

            memcpy(action->NewName, utf8filename.data(), utf8filename.size() * sizeof(String::value_type));
            action->NewName[utf8filename.size()] = L'\0';
            action->LastSize = 0;
            action->CheckForWriteStarted = false;

            return action;
        }

        static FileAction* CreateRenamed(const WString& oldFilename, const WString& newFileName)
        {
            String utf8Oldfilename = UTF8::FromWide(oldFilename);
            String utf8Newfilename = UTF8::FromWide(newFileName);

            UINT8* bytes = (UINT8*)te_allocate((UINT32)(sizeof(FileAction) +
                (utf8Oldfilename.size() + utf8Newfilename.size() + 2) * sizeof(String::value_type)));

            FileAction* action = (FileAction*)bytes;
            bytes += sizeof(FileAction);

            action->OldName = (String::value_type*)bytes;
            bytes += (utf8Oldfilename.size() + 1) * sizeof(String::value_type);

            action->NewName = (String::value_type*)bytes;
            action->Type = FileActionType::Renamed;

            memcpy(action->OldName, utf8Oldfilename.data(), utf8Oldfilename.size() * sizeof(String::value_type));
            action->OldName[utf8Oldfilename.size()] = L'\0';

            memcpy(action->NewName, utf8Newfilename.data(), utf8Newfilename.size() * sizeof(String::value_type));
            action->NewName[utf8Newfilename.size()] = L'\0';
            action->LastSize = 0;
            action->CheckForWriteStarted = false;

            return action;
        }

        static void destroy(FileAction* action)
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

        // No need for mutex since we know worker thread is shut down by now
        while (!m->FileActions.empty())
        {
            FileAction* action = m->FileActions.front();
            m->FileActions.pop();

            FileAction::destroy(action);
        }

        te_delete(m);
    }

    void FolderMonitor::StartMonitor(const String& folderPath, bool subdirectories, UINT32 changeFilter)
    { 
        if (!FileSystem::IsDirectory(folderPath))
        {
            TE_DEBUG("Provided path \"" + folderPath + "\" is not a directory");
            return;
        }

        WString extendedFolderPath = L"\\\\?\\" + UTF8::ToWide(BuildWindowsPath(std::filesystem::canonical(folderPath).string()));
        HANDLE dirHandle = CreateFileW(extendedFolderPath.c_str(), FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, nullptr);

        if (dirHandle == INVALID_HANDLE_VALUE)
        {
            TE_ASSERT_ERROR(false, "Failed to open folder \"" + folderPath + "\" for monitoring. Error code: " + ToString((UINT64)GetLastError()));
        }

        DWORD filterFlags = 0;

        if (changeFilter & (UINT32)FolderChangeFlag::FileName)
            filterFlags |= FILE_NOTIFY_CHANGE_FILE_NAME;

        if (changeFilter & (UINT32)FolderChangeFlag::DirName)
            filterFlags |= FILE_NOTIFY_CHANGE_DIR_NAME;

        if (changeFilter & (UINT32)FolderChangeFlag::FileWrite)
            filterFlags |= FILE_NOTIFY_CHANGE_LAST_WRITE;

        m->FoldersToWatch.push_back(te_new<FolderWatchInfo>(folderPath, dirHandle, subdirectories, filterFlags));
        FolderWatchInfo* watchInfo = m->FoldersToWatch.back();

        m->CompPortHandle = CreateIoCompletionPort(dirHandle, m->CompPortHandle, (ULONG_PTR)watchInfo, 0);

        if (m->CompPortHandle == nullptr)
        {
            m->FoldersToWatch.erase(m->FoldersToWatch.end() - 1);
            te_delete(watchInfo);
            TE_ASSERT_ERROR(false, "Failed to open completion port for folder monitoring. Error code: " + ToString((UINT64)GetLastError()));
        }

        if (m->WorkerThread == nullptr)
        {
            m->WorkerThread = te_new<Thread>(std::bind(&FolderMonitor::WorkerThreadMain, this));

            if (m->WorkerThread == nullptr)
            {
                m->FoldersToWatch.erase(m->FoldersToWatch.end() - 1);
                te_delete(watchInfo);
                TE_ASSERT_ERROR(false, "Failed to create a new worker thread for folder monitoring");
            }
        }

        if (m->WorkerThread != nullptr)
        {
            watchInfo->StartMonitor(m->CompPortHandle);
        }
        else
        {
            m->FoldersToWatch.erase(m->FoldersToWatch.end() - 1);
            te_delete(watchInfo);
            TE_ASSERT_ERROR(false, "Failed to create a new worker thread for folder monitoring");
        }
    }

    void FolderMonitor::StopMonitor(const String& folderPath)
    { 
        auto findIter = std::find_if(m->FoldersToWatch.begin(), m->FoldersToWatch.end(),
            [&](const FolderWatchInfo* x) { return x->FolderToMonitor == folderPath; });

        if (findIter != m->FoldersToWatch.end())
        {
            FolderWatchInfo* watchInfo = *findIter;

            watchInfo->StopMonitor(m->CompPortHandle);
            te_delete(watchInfo);

            m->FoldersToWatch.erase(findIter);
        }

        if (m->FoldersToWatch.size() == 0)
            StopMonitorAll();
    }

    void FolderMonitor::StopMonitorAll()
    {
        for (auto& watchInfo : m->FoldersToWatch)
        {
            watchInfo->StopMonitor(m->CompPortHandle);

            {
                // Note: Need this mutex to ensure worker thread is done with watchInfo.
                // Even though we wait for a condition variable from the worker thread in stopMonitor,
                // that doesn't mean the worker thread is done with the condition variable
                // (which is stored inside watchInfo)
                Lock lock(m->MainMutex);
                te_delete(watchInfo);
            }
        }

        m->FoldersToWatch.clear();

        if (m->WorkerThread != nullptr)
        {
            PostQueuedCompletionStatus(m->CompPortHandle, 0, 0, nullptr);

            m->WorkerThread->join();
            te_delete(m->WorkerThread);
            m->WorkerThread = nullptr;
        }

        if (m->CompPortHandle != nullptr)
        {
            CloseHandle(m->CompPortHandle);
            m->CompPortHandle = nullptr;
        }
    }

    void FolderMonitor::WorkerThreadMain()
    {
        FolderWatchInfo* watchInfo = nullptr;

        do
        {
            DWORD numBytes;
            LPOVERLAPPED overlapped;

            if (!GetQueuedCompletionStatus(m->CompPortHandle, &numBytes, (PULONG_PTR)&watchInfo, &overlapped, INFINITE))
            {
                assert(false);
                // TODO: Folder handle was lost most likely. Not sure how to deal with that. Shutdown watch on this folder and cleanup?
            }

            if (watchInfo != nullptr)
            {
                MonitorState state;

                {
                    Lock lock(watchInfo->StatusMutex);
                    state = watchInfo->State;
                }

                switch (state)
                {
                case MonitorState::Starting:
                    if (!ReadDirectoryChangesW(watchInfo->DirHandle, watchInfo->Buffer, FolderWatchInfo::READ_BUFFER_SIZE,
                        watchInfo->MonitorSubdirectories, watchInfo->MonitorFlags, &watchInfo->BufferSize, &watchInfo->Overlapped, nullptr))
                    {
                        assert(false); // TODO - Possibly the buffer was too small?
                        watchInfo->ReadError = GetLastError();
                    }
                    else
                    {
                        watchInfo->ReadError = ERROR_SUCCESS;

                        {
                            Lock lock(watchInfo->StatusMutex);
                            watchInfo->State = MonitorState::Monitoring;
                        }
                    }

                    watchInfo->StartStopEvent.notify_one();

                    break;
                case MonitorState::Monitoring:
                {
                    FileNotifyInfo info(watchInfo->Buffer, FolderWatchInfo::READ_BUFFER_SIZE);
                    HandleNotifications(info, *watchInfo);

                    if (!ReadDirectoryChangesW(watchInfo->DirHandle, watchInfo->Buffer, FolderWatchInfo::READ_BUFFER_SIZE,
                        watchInfo->MonitorSubdirectories, watchInfo->MonitorFlags, &watchInfo->BufferSize, &watchInfo->Overlapped, nullptr))
                    {
                        assert(false); // TODO: Failed during normal operation, possibly the buffer was too small. Shutdown watch on this folder and cleanup?
                        watchInfo->ReadError = GetLastError();
                    }
                    else
                    {
                        watchInfo->ReadError = ERROR_SUCCESS;
                    }
                }
                break;
                case MonitorState::Shutdown:
                    if (watchInfo->DirHandle != INVALID_HANDLE_VALUE)
                    {
                        CloseHandle(watchInfo->DirHandle);
                        watchInfo->DirHandle = INVALID_HANDLE_VALUE;

                        {
                            Lock lock(watchInfo->StatusMutex);
                            watchInfo->State = MonitorState::Shutdown2;
                        }
                    }
                    else
                    {
                        {
                            Lock lock(watchInfo->StatusMutex);
                            watchInfo->State = MonitorState::Inactive;
                        }

                        {
                            Lock lock(m->MainMutex); // Ensures that we don't delete "watchInfo" before this thread is done with mStartStopEvent
                            watchInfo->StartStopEvent.notify_one();
                        }
                    }

                    break;
                case MonitorState::Shutdown2:
                    if (watchInfo->DirHandle != INVALID_HANDLE_VALUE)
                    {
                        // Handle is still open? Try again.
                        CloseHandle(watchInfo->DirHandle);
                        watchInfo->DirHandle = INVALID_HANDLE_VALUE;
                    }
                    else
                    {
                        {
                            Lock lock(watchInfo->StatusMutex);
                            watchInfo->State = MonitorState::Inactive;
                        }

                        {
                            Lock lock(m->MainMutex); // Ensures that we don't delete "watchInfo" before this thread is done with mStartStopEvent
                            watchInfo->StartStopEvent.notify_one();
                        }
                    }

                    break;
                default:
                    break;
                }
            }

        } while (watchInfo != nullptr);
    }

    void FolderMonitor::HandleNotifications(FileNotifyInfo& notifyInfo, FolderWatchInfo& watchInfo)
    { 
        Vector<FileAction*> mActions;

        do
        {
            WString fullPath = notifyInfo.GetFileNameWithPath(watchInfo.FolderToMonitor);

            switch (notifyInfo.GetAction())
            {
            case FILE_ACTION_ADDED:
                mActions.push_back(FileAction::CreateAdded(fullPath));
                break;
            case FILE_ACTION_REMOVED:
                mActions.push_back(FileAction::CreateRemoved(fullPath));
                break;
            case FILE_ACTION_MODIFIED:
                mActions.push_back(FileAction::CreateModified(fullPath));
                break;
            case FILE_ACTION_RENAMED_OLD_NAME:
                watchInfo.CachedOldFileName = fullPath;  
                break;
            case FILE_ACTION_RENAMED_NEW_NAME:
                mActions.push_back(FileAction::CreateRenamed(watchInfo.CachedOldFileName, fullPath));
                break;
            }

        } while (notifyInfo.GetNext());

        {
            Lock lock(m->MainMutex);

            for (auto& action : mActions)
                m->FileActions.push(action);
        }
    }

    void FolderMonitor::Update()
    { 
        {
            Lock lock(m->MainMutex);

            while (!m->FileActions.empty())
            {
                FileAction* action = m->FileActions.front();
                m->FileActions.pop();

                m->ActiveFileActions.push_back(action);
            }
        }

        for (auto iter = m->ActiveFileActions.begin(); iter != m->ActiveFileActions.end();)
        {
            FileAction* action = *iter;

            // Reported file actions might still be in progress (i.e. something might still be writing to those files).
            // Sadly there doesn't seem to be a way to properly determine when those files are done being written, so instead
            // we check for at least a couple of frames if the file's size hasn't changed before reporting a file action.
            // This takes care of most of the issues and avoids reporting partially written files in almost all cases.
            if (FileSystem::Exists(action->NewName))
            {
                UINT64 size = FileSystem::GetFileSize(action->NewName);
                if (!action->CheckForWriteStarted)
                {
                    action->CheckForWriteStarted = true;
                    action->LastSize = size;

                    ++iter;
                    continue;
                }
                else
                {
                    if (action->LastSize != size)
                    {
                        action->LastSize = size;
                        ++iter;
                        continue;
                    }
                }
            }

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

            m->ActiveFileActions.erase(iter++);
            FileAction::destroy(action);
        }
    }
}
