#include "TeGLImGuiAPI.h"

#if TE_PLATFORM == TE_PLATFORM_WIN32
    #include "ImGui/imgui_impl_opengl3.h"
    #include "ImGui/imgui_impl_win32.h"
#else
    #include "ImGui/imgui_impl_glfw.h"
    #include "ImGui/imgui_impl_opengl3.h"
#endif

#if TE_PLATFORM == TE_PLATFORM_WIN32
#   pragma warning(push, 0)
#   include <Windows.h>
#   include <wingdi.h>
#   include <GL/glew.h>
#   include <GL/wglew.h>
#   pragma warning(pop)
#elif TE_PLATFORM == TE_PLATFORM_LINUX
#   include <GL/glew.h>
#   include <GL/glxew.h>
#   include <GL/glu.h>
#   define GL_GLEXT_PROTOTYPES
#endif

namespace te
{
    TE_MODULE_STATIC_MEMBER(GLImGuiAPI)

    GLImGuiAPI::GLImGuiAPI()
    { }

    GLImGuiAPI::~GLImGuiAPI()
    { }

    void GLImGuiAPI::Initialize(void* data)
    { 
        GuiAPI::Initialize(data);

#if TE_PLATFORM == TE_PLATFORM_WIN32

#else

#endif
    }

    void GLImGuiAPI::Destroy()
    {
        GuiAPI::Destroy();
    }

    void GLImGuiAPI::Update()
    {
        GuiAPI::Update();
    }

    void GLImGuiAPI::BeginFrame()
    { }

    void GLImGuiAPI::EndFrame()
    { }

    bool GLImGuiAPI::HasFocus(FocusType type)
    {
        return false;
    }

    void GLImGuiAPI::CharInput(UINT32 character)
    { }

    void GLImGuiAPI::CursorMoved(const Vector2I& cursorPos, const OSPointerButtonStates& btnStates)
    { }

    void GLImGuiAPI::CursorPressed(const Vector2I& cursorPos, OSMouseButton button, const OSPointerButtonStates& btnStates)
    { }

    void GLImGuiAPI::CursorReleased(const Vector2I& cursorPos, OSMouseButton button, const OSPointerButtonStates& btnStates)
    { }

    void GLImGuiAPI::MouseWheelScrolled(float scrollPos)
    { }

    void GLImGuiAPI::KeyUp(UINT32 keyCode)
    { }

    void GLImGuiAPI::KeyDown(UINT32 keyCode)
    { }

    void GLImGuiAPI::CursorDoubleClick(const Vector2I& cursorPos, const OSPointerButtonStates& btnStates)
    { }
}
