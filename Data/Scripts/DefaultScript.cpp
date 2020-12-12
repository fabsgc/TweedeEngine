#include "Scripting/TeNativeScript.h"

using namespace te;

class DefaultScript : public NativeScript
{
    ~DefaultScript()
    { 
        TE_PRINT("delete");
    }

    virtual void Update() override
    { }
};

extern "C" TE_SCRIPT_EXPORT NativeScript* LoadScript()
{
    return new DefaultScript();
}

extern "C" TE_SCRIPT_EXPORT void UnloadScript(NativeScript* script)
{
    if(script)
        delete script;
}

// Command for compiling
// cl.exe /LD /IC:\Users\FBN6\Documents\Code\TweedeFrameworkRedux\Source\Framework\Core /IC:\Users\FBN6\Documents\Code\TweedeFrameworkRedux\Source\Framework\Utility /Zi /nologo /W3 /WX- /diagnostics:classic /MP /O2 /Ob2 /Oi /GL /D WIN32 /D _WINDOWS /D NDEBUG /D_WINDLL /D _MBCS /Gm- /MD /GS- /Gy /fp:precise /Zc:wchar_t /Zc:forScope /Zc:inline /GR /std:c++17 "Data/Scripts/DefaultScript.cpp" /Gd /TP /wd4577 /wd4530 /bigobj /link /OPT:REF ..\..\..\lib\x64\Debug\tef.lib /DEBUG /PDB:"DefaultScript.pdb" /LTCG /TLBID:1 /DYNAMICBASE /NXCOMPAT /MACHINE:X64  /machine:x64