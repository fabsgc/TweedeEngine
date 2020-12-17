#include "Scripting/TeScriptManager.h"
#include "Utility/TeFileSystem.h"

#include <windows.h>
#include <tchar.h>

namespace te
{
    String CompileDebug(const String& name)
    {
        String output;
        static String librariesPath = ReplaceAll(ScriptManager::LIBRARIES_PATH, "/", "\\");
#ifdef TE_ENGINE_BUILD
        static String rawAppRoot = ReplaceAll(RAW_APP_ROOT, "/", "\\");
#else
        static String rawAppRoot = "";
#endif

        output += "/LD ";
        output += "/I" + rawAppRoot + "Source\\Framework\\Core  ";
        output += "/I" + rawAppRoot + "Source\\Framework\\Utility  ";
        output += "/ZI /nologo /W3 /WX- /diagnostics:classic /MP /Od /Ob0 /D WIN32 /D _WINDOWS ";
        output += "/D DEBUG /D _WINDLL /D _MBCS /Gm- /RTC1 /MDd /GS- /fp:precise /Zc:wchar_t ";
        output += "/Zc:forScope /Zc:inline /GR- /std:c++17 ";
        output += rawAppRoot + librariesPath + name + ".cpp ";
        output += "/Gd /TP /wd4577 /wd4530 /bigobj /link ";
        output += "/OUT:" + name + ".dll ";
        output += "/OPT:NOREF /OPT:NOICF ";
#if TE_CONFIG == TE_CONFIG_DEBUG
        output += "..\\..\\..\\lib\\x64\\Debug\\tef.lib ";
#elif  TE_CONFIG == TE_CONFIG_RELWITHDEBINFO
        output += "..\\..\\..\\lib\\x64\\RelWithDebInfo\\tef.lib ";
#elif  TE_CONFIG == TE_CONFIG_MINSIZEREL
        output += "..\\..\\..\\lib\\x64\\MinSizeRel\\tef.lib ";
#endif
        output += "/DEBUG ";
        output += "/PDB:" + name + ".pdb" + " ";
        output += "/TLBID:1 /DYNAMICBASE /NXCOMPAT /MACHINE:X64 /DEBUG /machine:x64";

        return output;
    }

    String CompileRelease(const String& name)
    {
        String output;
        static String librariesPath = ReplaceAll(ScriptManager::LIBRARIES_PATH, "/", "\\");
#ifdef TE_ENGINE_BUILD
        static String rawAppRoot = ReplaceAll(RAW_APP_ROOT, "/", "\\");
#else
        static String rawAppRoot = "";
#endif

        output += "/LD ";
        output += "/I" + rawAppRoot + "Source\\Framework\\Core  ";
        output += "/I" + rawAppRoot + "Source\\Framework\\Utility  ";
        output += "/Zi /nologo /W3 /WX- /diagnostics:classic /MP /Od /D WIN32 /D _WINDOWS ";
        output += "/D NDEBUG /D_WINDLL /D _MBCS /Gm- /MD /GS- /Gy /fp:precise /Zc:wchar_t ";
        output += "/Zc:forScope /Zc:inline /GR /std:c++17 ";
        output += rawAppRoot + librariesPath + name + ".cpp ";
        output += "/Gd /TP /wd4577 /wd4530 /bigobj /link ";
        output += "/OUT:" + name + ".dll ";
        output += "/OPT:REF ";
#if  TE_CONFIG == TE_CONFIG_RELWITHDEBINFO
        output += "..\\..\\..\\lib\\x64\\RelWithDebInfo\\tef.lib ";
#elif  TE_CONFIG == TE_CONFIG_MINSIZEREL
        output += "..\\..\\..\\lib\\x64\\MinSizeRel\\tef.lib ";
#elif  TE_CONFIG == TE_CONFIG_RELEASE
        output += "..\\..\\..\\lib\\x64\\Release\\tef.lib ";
#endif
        output += "/DEBUG ";
        output += "/PDB:" + name + ".pdb" + " ";
        output += "/TLBID:1 /DYNAMICBASE /NXCOMPAT /MACHINE:X64 /DEBUG /machine:x64";

        return output;
    }

    bool ScriptManager::CompileLibrary(const String& name)
    {
        bool retVal = true;
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        static String cxxCompilerPath = ReplaceAll(CXX_COMPILER_PATH, "/", "\\");
        static String msvcVars = ReplaceAll(MSVC_VCVARS, "/", "\\");

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        String directory = FileSystem::GetWorkingDirectoryPath();
        String command = "\"" + msvcVars + "\" x64 && \"" + cxxCompilerPath + "\" ";
        UINT32 flags = 0;

#if TE_DEBUG_MODE
        command = command + CompileDebug(name);
#else
        command = command + CompileRelease(name);
        flags |= CREATE_NO_WINDOW;
#endif

        if (CreateProcess(NULL,                  // No module name (use command line)
            const_cast<LPSTR>(command.c_str()),  // Command line
            NULL,                                // Process handle not inheritable
            NULL,                                // Thread handle not inheritable
            FALSE,                               // Set handle inheritance to FALSE
            flags,                               // No creation flags
            NULL,                                // Use parent's environment block
            directory.c_str(),                   // Use parent's starting directory 
            &si,                                 // Pointer to STARTUPINFO structure
            &pi)                                 // Pointer to PROCESS_INFORMATION structure
            )
        {
            retVal =  true;

            // Wait until child process exits.
            WaitForSingleObject(pi.hProcess, INFINITE);

            // Close process and thread handles. 
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
        else
        {
            DWORD errorCode = GetLastError();

            TE_DEBUG("Failed to compile library \"" + name + "\", error : " + ToString(UINT32(errorCode)));
            retVal = false;
        }

        return retVal;
    }
}
