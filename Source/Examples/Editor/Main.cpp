#include "TeApplication.h"
#include "RenderAPI/TeVideoMode.h"

#if TE_PLATFORM == TE_PLATFORM_WIN32
#include <windows.h>

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

    return 0;
}
