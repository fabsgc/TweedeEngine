#include "Utility/TeFileSystem.h"
#include "String/TeUnicode.h"

#include <functional>

namespace te
{
    void win32_handleError(DWORD error, const WString& path)
    {
        switch (error)
        {
        case ERROR_FILE_NOT_FOUND:
            TE_DEBUG("File at path: \"" + UTF8::FromWide(path) + "\" not found.");
            break;
        case ERROR_PATH_NOT_FOUND:
        case ERROR_BAD_NETPATH:
        case ERROR_CANT_RESOLVE_FILENAME:
        case ERROR_INVALID_DRIVE:
            TE_DEBUG("Path \"" + UTF8::FromWide(path) + "\" not found.");
            break;
        case ERROR_ACCESS_DENIED:
            TE_DEBUG("Access to path \"" + UTF8::FromWide(path) + "\" denied.");
            break;
        case ERROR_ALREADY_EXISTS:
        case ERROR_FILE_EXISTS:
            TE_DEBUG("File/folder at path \"" + UTF8::FromWide(path) + "\" already exists.");
            break;
        case ERROR_INVALID_NAME:
        case ERROR_DIRECTORY:
        case ERROR_FILENAME_EXCED_RANGE:
        case ERROR_BAD_PATHNAME:
            TE_DEBUG("Invalid path string: \"" + UTF8::FromWide(path) + "\".");
            break;
        case ERROR_FILE_READ_ONLY:
            TE_DEBUG("File at path \"" + UTF8::FromWide(path) + "\" is read only.");
            break;
        case ERROR_CANNOT_MAKE:
            TE_DEBUG("Cannot create file/folder at path: \"" + UTF8::FromWide(path) + "\".");
            break;
        case ERROR_DIR_NOT_EMPTY:
            TE_DEBUG("Directory at path \"" + UTF8::FromWide(path) + "\" not empty.");
            break;
        case ERROR_WRITE_FAULT:
            TE_DEBUG("Error while writing a file at path \"" + UTF8::FromWide(path) + "\".");
            break;
        case ERROR_READ_FAULT:
            TE_DEBUG("Error while reading a file at path \"" + UTF8::FromWide(path) + "\".");
            break;
        case ERROR_SHARING_VIOLATION:
            TE_DEBUG("Sharing violation at path \"" + UTF8::FromWide(path) + "\".");
            break;
        case ERROR_LOCK_VIOLATION:
            TE_DEBUG("Lock violation at path \"" + UTF8::FromWide(path) + "\".");
            break;
        case ERROR_HANDLE_EOF:
            TE_DEBUG("End of file reached for file at path \"" + UTF8::FromWide(path) + "\".");
            break;
        case ERROR_HANDLE_DISK_FULL:
        case ERROR_DISK_FULL:
            TE_DEBUG("Disk full.");
            break;
        case ERROR_NEGATIVE_SEEK:
            TE_DEBUG("Negative seek.");
            break;
        default:
            TE_DEBUG("Undefined file system exception: " + ToString((UINT32)error));
            break;
        }
    }

    bool win32_pathExists(const WString& path)
    {
        DWORD attr = GetFileAttributesW(path.c_str());
        if (attr == 0xFFFFFFFF)
        {
            switch (GetLastError())
            {
            case ERROR_FILE_NOT_FOUND:
            case ERROR_PATH_NOT_FOUND:
            case ERROR_NOT_READY:
            case ERROR_INVALID_DRIVE:
                return false;
            default:
                win32_handleError(GetLastError(), path);
            }
        }
        return true;
    }

    bool win32_isDirectory(const WString& path)
    {
        DWORD attr = GetFileAttributesW(path.c_str());
        if (attr == 0xFFFFFFFF)
            win32_handleError(GetLastError(), path);

        return (attr & FILE_ATTRIBUTE_DIRECTORY) != FALSE;
    }

    bool win32_isDevice(const WString& path)
    {
        WString ucPath = path;
        Util::ToUpperCase(ucPath);

        return
            ucPath.compare(0, 4, L"\\\\.\\") == 0 ||
            ucPath.compare(L"CON") == 0 ||
            ucPath.compare(L"PRN") == 0 ||
            ucPath.compare(L"AUX") == 0 ||
            ucPath.compare(L"NUL") == 0 ||
            ucPath.compare(L"LPT1") == 0 ||
            ucPath.compare(L"LPT2") == 0 ||
            ucPath.compare(L"LPT3") == 0 ||
            ucPath.compare(L"LPT4") == 0 ||
            ucPath.compare(L"LPT5") == 0 ||
            ucPath.compare(L"LPT6") == 0 ||
            ucPath.compare(L"LPT7") == 0 ||
            ucPath.compare(L"LPT8") == 0 ||
            ucPath.compare(L"LPT9") == 0 ||
            ucPath.compare(L"COM1") == 0 ||
            ucPath.compare(L"COM2") == 0 ||
            ucPath.compare(L"COM3") == 0 ||
            ucPath.compare(L"COM4") == 0 ||
            ucPath.compare(L"COM5") == 0 ||
            ucPath.compare(L"COM6") == 0 ||
            ucPath.compare(L"COM7") == 0 ||
            ucPath.compare(L"COM8") == 0 ||
            ucPath.compare(L"COM9") == 0;
    }

    bool win32_isFile(const WString& path)
    {
        return !win32_isDirectory(path) && !win32_isDevice(path);
    }

    UINT64 win32_getFileSize(const WString& path)
    {
        WIN32_FILE_ATTRIBUTE_DATA attrData;
        if (GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &attrData) == FALSE)
            win32_handleError(GetLastError(), path);

        LARGE_INTEGER li;
        li.LowPart = attrData.nFileSizeLow;
        li.HighPart = attrData.nFileSizeHigh;
        return (UINT64)li.QuadPart;
    }

    std::time_t win32_getLastModifiedTime(const WString& path)
    {
        WIN32_FILE_ATTRIBUTE_DATA fad;
        if (GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &fad) == 0)
            win32_handleError(GetLastError(), path);

        ULARGE_INTEGER ull;
        ull.LowPart = fad.ftLastWriteTime.dwLowDateTime;
        ull.HighPart = fad.ftLastWriteTime.dwHighDateTime;

        return (std::time_t) ((ull.QuadPart / 10000000ULL) - 11644473600ULL);
    }

    WString win32_getCurrentDirectory()
    {
        DWORD len = GetCurrentDirectoryW(0, NULL);
        if (len > 0)
        {
            wchar_t* buffer = (wchar_t*)te_allocate(len * sizeof(wchar_t));

            DWORD n = GetCurrentDirectoryW(len, buffer);
            if (n > 0 && n <= len)
            {
                WString result(buffer);
                if (result[result.size() - 1] != L'\\')
                    result.append(L"\\");

                te_free(buffer);
                return result;
            }

            te_free(buffer);
        }

        return WString();
    }

    UINT64 FileSystem::GetFileSize(const String& fullPath)
    {
        return win32_getFileSize(UTF8::ToWide(fullPath));
    }

    void FileSystem::GetChildren(const String& dirPath, Vector<String>& files, Vector<String>& directories, bool onlyFileName)
    {
        WString findPath = UTF8::ToWide(dirPath);

        if (win32_isFile(findPath))
            return;

        if(IsFile(dirPath)) // Assuming the file is a folder, just improperly formatted in Path
            findPath.append(L"\\*");
        else
            findPath.append(L"*");

        WIN32_FIND_DATAW findData;
        HANDLE fileHandle = FindFirstFileW(findPath.c_str(), &findData);
        if(fileHandle == INVALID_HANDLE_VALUE)
        {
            win32_handleError(GetLastError(), findPath);
            return;
        }

        WString tempName;
        do
        {
            tempName = findData.cFileName;

            if (tempName != L"." && tempName != L"..")
            {
                String fullPath;
                if (!onlyFileName) fullPath = dirPath;
                if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
                    directories.push_back(fullPath.append(UTF8::FromWide(tempName) + u8"/"));
                else
                    files.push_back(fullPath.append(UTF8::FromWide(tempName)));
            }

            if(FindNextFileW(fileHandle, &findData) == FALSE)
            {
                if (GetLastError() != ERROR_NO_MORE_FILES)
                    win32_handleError(GetLastError(), findPath);

                break;
            }
        } while (true);

        FindClose(fileHandle);
    }

    bool FileSystem::Iterate(const String& dirPath, std::function<bool(const String&)> fileCallback, std::function<bool(const String&)> dirCallback, bool recursive)
    {
        WString findPath = UTF8::ToWide(dirPath);

        if (win32_isFile(findPath))
            return false;

        if (IsFile(dirPath)) // Assuming the file is a folder, just improperly formatted in Path
            findPath.append(L"\\*");
        else
            findPath.append(L"*");

        WIN32_FIND_DATAW findData;
        HANDLE fileHandle = FindFirstFileW(findPath.c_str(), &findData);
        if (fileHandle == INVALID_HANDLE_VALUE)
        {
            win32_handleError(GetLastError(), findPath);
            return false;
        }

        WString tempName;
        do
        {
            tempName = findData.cFileName;

            if (tempName != L"." && tempName != L"..")
            {
                String fullPath = dirPath;
                if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
                {
                    String childDir = fullPath.append(UTF8::FromWide(tempName) + u8"/");
                    if (dirCallback != nullptr)
                    {
                        if (!dirCallback(childDir))
                        {
                            FindClose(fileHandle);
                            return false;
                        }
                    }

                    if (recursive)
                    {
                        if (!Iterate(childDir, fileCallback, dirCallback, recursive))
                        {
                            FindClose(fileHandle);
                            return false;
                        }
                    }
                }
                else
                {
                    String filePath = fullPath.append(UTF8::FromWide(tempName));
                    if (fileCallback != nullptr)
                    {
                        if (!fileCallback(filePath))
                        {
                            FindClose(fileHandle);
                            return false;
                        }
                    }
                }
            }

            if (FindNextFileW(fileHandle, &findData) == FALSE)
            {
                if (GetLastError() != ERROR_NO_MORE_FILES)
                    win32_handleError(GetLastError(), findPath);

                break;
            }
        } while (true);

        FindClose(fileHandle);
        return true;
    }

    std::time_t FileSystem::GetLastModifiedTime(const String& fullPath)
    {
        return win32_getLastModifiedTime(UTF8::ToWide(fullPath));
    }

    String FileSystem::GetWorkingDirectoryPath()
    {
        const String utf8dir = UTF8::FromWide(win32_getCurrentDirectory());
        return utf8dir;
    }
}
