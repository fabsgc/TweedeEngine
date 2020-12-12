#include "Scripting/TeScriptManager.h"
#include "Utility/TeFileSystem.h"

#include <windows.h>
#include <tchar.h>

namespace te
{
    bool ScriptManager::CompileLibrary(const String& name)
    {
        bool retVal = true;
        STARTUPINFO si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        String command = "cl.exe";

        if (CreateProcess(NULL,                  // No module name (use command line)
            const_cast<LPSTR>(command.c_str()),  // Command line
            NULL,                                // Process handle not inheritable
            NULL,                                // Thread handle not inheritable
            FALSE,                               // Set handle inheritance to FALSE
            0,                                   // No creation flags
            NULL,                                // Use parent's environment block
            NULL,                                // Use parent's starting directory 
            &si,                                 // Pointer to STARTUPINFO structure
            &pi)                                 // Pointer to PROCESS_INFORMATION structure
            )
        {
            retVal =  true;
        }
        else
        {
            TE_DEBUG("Failed to compile library \"" + name + "\""); 
            retVal = false;
        }

        return retVal;
    }
}
