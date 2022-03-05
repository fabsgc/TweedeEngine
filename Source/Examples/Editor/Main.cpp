#include "TeApplication.h"
#include "RenderAPI/TeVideoMode.h"

#if TE_PLATFORM == TE_PLATFORM_WIN32
#include <windows.h>

#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
//#   define _CRTDBG_MAP_ALLOC
//#   include <stdlib.h>
//#   include <crtdbg.h>
#endif

int CALLBACK WinMain(
    _In_  HINSTANCE hInstance,
    _In_  HINSTANCE hPrevInstance,
    _In_  LPSTR lpCmdLine,
    _In_  int nCmdShow
)  
#else
int main()
#endif
{
    te::START_UP_DESC desc;

    desc.RenderAPI = TE_RENDER_API_MODULE;
    desc.Renderer = TE_RENDERER_MODULE;
    desc.Physics = TE_PHYSICS_MODULE;
    desc.Audio = TE_AUDIO_MODULE;
    desc.Gui = TE_GUI_MODULE;

    desc.Importers = {
        "TeFreeImgImporter",
        "TeObjectImporter",
        "TeShaderImporter",
        "TeFontImporter"
    };

    desc.WindowDesc.Mode = te::VideoMode(1280, 720);
    desc.WindowDesc.Fullscreen = false;
    desc.WindowDesc.MultisampleCount = 1; //MSAA is useless for an editor
    desc.WindowDesc.Title = "Editor";
    desc.WindowDesc.Vsync = false;

    te::Application::StartUp(desc);
    te::Application::Instance().RunMainLoop();
    te::Application::ShutDown();

#if TE_PLATFORM == TE_PLATFORM_WIN32 && TE_DEBUG_MODE == TE_DEBUG_ENABLED
    /*HANDLE hLogFile = CreateFile("MemoryLeaks.txt", GENERIC_WRITE, FILE_SHARE_WRITE,
        NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    //Turn on debugging for memory leaks. This is automatically turned off when the build is Release.
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, hLogFile);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, hLogFile);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, hLogFile);

    _CrtDumpMemoryLeaks();*/
#endif

    return 0;
}
