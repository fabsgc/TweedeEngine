set (TE_GLIMGUIAPI_INC_NOFILTER
    "TeGLImGuiAPIPrerequisites.h"
    "TeGLImGuiAPIFactory.h"
    "TeGLImGuiAPI.h"
)

set (TE_GLIMGUIAPI_SRC_NOFILTER
    "TeGLImGuiAPIFactory.cpp"
    "TeGLImGuiAPI.cpp"
    "TeGLImGuiAPIPlugin.cpp"
    "glew.cpp"
)

set(TE_GLIMGUIAPI_INC_IMGUI
    "ImGui/imgui_impl_opengl3.h"
)

set(TE_GLIMGUIAPI_SRC_IMGUI
    "ImGui/imgui_impl_opengl3.cpp"
)

if(WIN32)
    list(APPEND TE_GLIMGUIAPI_INC_IMGUI "ImGui/imgui_impl_win32.h")
    list(APPEND TE_GLIMGUIAPI_SRC_IMGUI "ImGui/imgui_impl_win32.cpp")
elseif(LINUX)
    list(APPEND TE_GLIMGUIAPI_INC_IMGUI "ImGui/imgui_impl_glfw.h")
    list(APPEND TE_GLIMGUIAPI_SRC_IMGUI "ImGui/imgui_impl_glfw.cpp")
endif()

source_group ("" FILES ${TE_GLIMGUIAPI_SRC_NOFILTER} ${TE_GLIMGUIAPI_INC_NOFILTER})
source_group ("ImGui" FILES ${TE_GLIMGUIAPI_SRC_IMGUI} ${TE_GLIMGUIAPI_INC_IMGUI})

set (TE_GLIMGUIAPI_SRC
    ${TE_GLIMGUIAPI_INC_NOFILTER}
    ${TE_GLIMGUIAPI_SRC_NOFILTER}
    ${TE_GLIMGUIAPI_INC_IMGUI}
    ${TE_GLIMGUIAPI_SRC_IMGUI}
)
