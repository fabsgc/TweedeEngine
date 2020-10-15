#include "TeD3D11ImGuiAPI.h"
#include "TeCoreApplication.h"
#include "Platform/TePlatform.h"

namespace te
{
    struct ImGuiData
    {
        HWND HWnd;
        ID3D11Device* PD3D11Device;
        ID3D11DeviceContext* PD3D11DeviceContext;
    };

    D3D11ImGuiAPI::D3D11ImGuiAPI()
    { }

    D3D11ImGuiAPI::~D3D11ImGuiAPI()
    { }

    void D3D11ImGuiAPI::Initialize(void* data)
    {
        GuiAPI::Initialize(data);

        ImGuiData* guiData = static_cast<ImGuiData*>(data);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        io.ConfigDockingTransparentPayload = true;

        ImGui::StyleColorsDark();

        ImGui_ImplWin32_Init(guiData->HWnd);
        ImGui_ImplDX11_Init(guiData->PD3D11Device, guiData->PD3D11DeviceContext);

        _guiInitialized = true;
    }

    void D3D11ImGuiAPI::Destroy()
    {
        GuiAPI::Destroy();

        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    void D3D11ImGuiAPI::Update()
    {
        GuiAPI::Update();
    }

    void D3D11ImGuiAPI::BeginFrame()
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void D3D11ImGuiAPI::EndFrame()
    {
        ImGuiIO& io = ImGui::GetIO();
        UINT32 width = gCoreApplication().GetWindow()->GetProperties().Width;
        UINT32 height = gCoreApplication().GetWindow()->GetProperties().Height;

        io.DisplaySize = ImVec2((float)width, (float)height);

        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    bool D3D11ImGuiAPI::HasFocus(FocusType type)
    {
        ImGuiIO& io = ImGui::GetIO();
        if (type == FocusType::Keyboard && io.WantCaptureKeyboard) return true;
        if (type == FocusType::Mouse && io.WantCaptureMouse) return true;

        return false;
    }

    void D3D11ImGuiAPI::CharInput(UINT32 character)
    {
        ImGuiIO& io = ImGui::GetIO();

        if (character > 0 && character < 0x10000)
            io.AddInputCharacterUTF16((unsigned short)character);
    }

    void D3D11ImGuiAPI::CursorMoved(const Vector2I& cursorPos, const OSPointerButtonStates& btnStates)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.MousePos.x = (float)cursorPos.x;
        io.MousePos.y = (float)cursorPos.y;
    }

    void D3D11ImGuiAPI::CursorPressed(const Vector2I& cursorPos, OSMouseButton button, const OSPointerButtonStates& btnStates)
    {
        ImGuiIO& io = ImGui::GetIO();

        switch (button)
        {
        case OSMouseButton::Left: { io.MouseDown[0] = true; } break;
        case OSMouseButton::Middle: { io.MouseDown[2] = true; } break;
        case OSMouseButton::Right: { io.MouseDown[1] = true; } break;
        }
    }

    void D3D11ImGuiAPI::CursorReleased(const Vector2I& cursorPos, OSMouseButton button, const OSPointerButtonStates& btnStates)
    {
        ImGuiIO& io = ImGui::GetIO();

        switch (button)
        {
        case OSMouseButton::Left: { io.MouseDown[0] = false; } break;
        case OSMouseButton::Middle: { io.MouseDown[2] = false; } break;
        case OSMouseButton::Right: { io.MouseDown[1] = false; } break;
        }
    }

    void D3D11ImGuiAPI::MouseWheelScrolled(float scrollPos)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.MouseWheel = scrollPos;
    }

    void D3D11ImGuiAPI::KeyUp(UINT32 keyCode)
    { 
        if (keyCode > 256)
            return;

        ImGuiIO& io = ImGui::GetIO();
        io.KeysDown[keyCode] = false;
    }

    void D3D11ImGuiAPI::KeyDown(UINT32 keyCode)
    {
        if (keyCode > 256)
            return;

        ImGuiIO& io = ImGui::GetIO();
        io.KeysDown[keyCode] = true;
    }
}
