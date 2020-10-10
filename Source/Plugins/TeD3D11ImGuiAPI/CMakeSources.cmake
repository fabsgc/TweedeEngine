set (TE_D3D11IMGUIAPI_INC_NOFILTER
    "TeD3D11ImGuiAPIPrerequisites.h"
    "TeD3D11ImGuiAPIFactory.h"
    "TeD3D11ImGuiAPI.h"
)

set (TE_D3D11IMGUIAPI_SRC_NOFILTER
    "TeD3D11ImGuiAPIFactory.cpp"
    "TeD3D11ImGuiAPI.cpp"
    "TeD3D11ImGuiAPIPlugin.cpp"
)

set(TE_D3D11IMGUIAPI_INC_IMGUI
    "ImGui/imgui_impl_dx11.h"
    "ImGui/imgui_impl_win32.h"
    "ImGui/imgui.h"
    "ImGui/imconfig.h"
    "ImGui/imgui_internal.h"
    "ImGui/imstb_rectpack.h"
    "ImGui/imstb_textedit.h"
    "ImGui/imstb_truetype.h"
)

set(TE_D3D11IMGUIAPI_SRC_IMGUI
    "ImGui/imgui_impl_dx11.cpp"
    "ImGui/imgui_impl_win32.cpp"
    "ImGui/imgui.cpp"
    "ImGui/imgui_demo.cpp"
    "ImGui/imgui_draw.cpp"
    "ImGui/imgui_widgets.cpp"
)

source_group ("" FILES ${TE_D3D11IMGUIAPI_SRC_NOFILTER} ${TE_D3D11IMGUIAPI_INC_NOFILTER})
source_group ("ImGui" FILES ${TE_D3D11IMGUIAPI_SRC_IMGUI} ${TE_D3D11IMGUIAPI_INC_IMGUI})

set (TE_D3D11IMGUIAPI_SRC
    ${TE_D3D11IMGUIAPI_INC_NOFILTER}
    ${TE_D3D11IMGUIAPI_SRC_NOFILTER}
    ${TE_D3D11IMGUIAPI_INC_IMGUI}
    ${TE_D3D11IMGUIAPI_SRC_IMGUI}
)
