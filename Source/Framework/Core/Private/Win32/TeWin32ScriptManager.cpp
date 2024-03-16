#include "Scripting/TeScriptManager.h"

#include "Scripting/TeScript.h"
#include "Utility/TeFileSystem.h"

#include <tchar.h>

namespace te
{
    String CompileDebug(const Script* script)
    {
        String output;
        static String compiler = TE_PLATFORM_COMPILER;
#ifdef TE_ENGINE_BUILD
        static String appRoot = ReplaceAll(RAW_APP_ROOT, "/", "\\");
        static String includePath = appRoot + "Source\\Framework\\";
#else
        static String appRoot = "";
        static String includePath = appRoot + "Include\\";
#endif

#if TE_COMPILER == TE_COMPILER_MSVC
        output += "/LD ";
        output += "/I" + includePath + "Core ";
        output += "/I" + includePath + "Utility ";
        output += "/ZI /nologo /W3 /WX- /diagnostics:classic /MP /Od /Ob0 /D WIN32 /D _WINDOWS ";
        output += "/D DEBUG /D _WINDLL /D _MBCS /Gm- /RTC1 /MDd /GS- /fp:precise /Zc:wchar_t ";
        output += "/Zc:forScope /Zc:inline /GR- /std:c++17 ";
        output += script->GetPath() + " ";
        output += "/Gd /TP /wd4577 /wd4530 /bigobj /link ";
        output += "/OUT:" + script->GetUUID().ToString() + ".dll ";
        output += "/OPT:NOREF /OPT:NOICF ";
#   if TE_CONFIG == TE_CONFIG_DEBUG
        output += "..\\..\\lib\\" + compiler + ".Debug.x64\\tef.lib ";
#   elif  TE_CONFIG == TE_CONFIG_RELWITHDEBINFO
        output += "..\\..\\lib\\" + compiler + ".RelWithDebInfo.x64\\tef.lib ";
#   elif  TE_CONFIG == TE_CONFIG_MINSIZEREL
        output += "..\\..\\lib\\" + compiler + ".MinSizeRel.x64\\tef.lib ";
#   endif
        output += "/DEBUG ";
        output += "/PDB:" + script->GetUUID().ToString() + ".pdb" + " ";
        output += "/TLBID:1 /DYNAMICBASE /NXCOMPAT /MACHINE:X64 /DEBUG /machine:x64";
#elif TE_COMPILER == TE_COMPILER_GNUC
        // TODO GNU_WIN32
#endif

        return output;
    }

    String CompileRelease(const Script* script)
    {
        String output;
        return output;
    }

    bool ScriptManager::CompileLibrary(const Script* script)
    {
        if (script->GetPath().empty())
            return false;

        if (!CheckLastBuildOldEnough(script))
            return true;

        bool retVal = true;
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        static String cxxCompilerPath = ReplaceAll(CXX_COMPILER_PATH, "/", "\\");
        static String msvcVars = ReplaceAll(MSVC_VCVARS, "/", "\\");

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        String directory = FileSystem::GetWorkingDirectoryPath();

#if TE_COMPILER == TE_COMPILER_MSVC
        String command = "\"" + msvcVars + "\" x64 && \"" + cxxCompilerPath + "\" ";
        UINT32 flags = 0;
#elif TE_COMPILER == TE_COMPILER_GNUC
        String command = "\"" + cxxCompilerPath + "\" ";
        UINT32 flags = 0;
        return false; // TODO GNU_WIN32
#endif

#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
        command = command + CompileDebug(script);
#else
        command = command + CompileRelease(identifier);
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
            retVal = true;

            // Wait until child process exits.
            WaitForSingleObject(pi.hProcess, INFINITE);

            // Close process and thread handles. 
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);

            _lastBuildTimes[script] = gTime().GetTimeMs();
        }
        else
        {
            DWORD errorCode = GetLastError();

            TE_DEBUG("Failed to compile library \"" + script->GetName() + "\", error : " + ToString(UINT32(errorCode)));
            retVal = false;
        }

        return retVal;
    }
}
