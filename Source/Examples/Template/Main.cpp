#include "TeApplication.h"
#include "RenderAPI/TeVideoMode.h"

#if TE_PLATFORM == TE_PLATFORM_WIN32
int CALLBACK WinMain(
    _In_  HINSTANCE hInstance,
    _In_opt_  HINSTANCE hPrevInstance,
    _In_  LPSTR lpCmdLine,
    _In_  int nCmdShow
)  
#else
int main()
#endif
{
    te::START_UP_DESC desc;

    desc.RenderAPI = TE_RENDER_API_MODULE;
    //desc.RenderAPI = "TeGLRenderAPI";
    desc.Renderer = TE_RENDERER_MODULE;
    desc.Physics = TE_PHYSICS_MODULE;
    desc.Audio = TE_AUDIO_MODULE;
    desc.Gui = "TeGLImGuiAPI";

    desc.Importers = {
        "TeFreeImgImporter",
        "TeObjectImporter",
        "TeShaderImporter",
        "TeFontImporter",
        "TeProjectImporter"
        "TeResourceImporter"
    };

    desc.Exporters = {
        "TeProjectExporter",
        "TeResourceExporter",
        "TeFreeImgExporter"
    };

    desc.WindowDesc.Mode = te::VideoMode(1024, 576);
    desc.WindowDesc.Fullscreen = false;
    desc.WindowDesc.MultisampleCount = 1;
    desc.WindowDesc.Title = "Template";

    te::Application::StartUp(desc);
    te::Application::Instance().RunMainLoop();
    te::Application::ShutDown();

    return 0;
}
