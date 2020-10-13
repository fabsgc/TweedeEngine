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

        ImGuiData* guiData = (ImGuiData*)data;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui::StyleColorsDark();

        ImGui_ImplWin32_Init(guiData->HWnd);
        ImGui_ImplDX11_Init(guiData->PD3D11Device, guiData->PD3D11DeviceContext);
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

        bool open = true;

        ImGui::ShowDemoWindow(&open);

        ImGuiIO& io = ImGui::GetIO();
        io.WantCaptureMouse = true;
        io.WantCaptureKeyboard = true;
        io.WantTextInput = true;
    }

    void D3D11ImGuiAPI::Begin()
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void D3D11ImGuiAPI::End()
    {
        ImGuiIO& io = ImGui::GetIO();
        UINT32 width = gCoreApplication().GetWindow()->GetProperties().Width;
        UINT32 height = gCoreApplication().GetWindow()->GetProperties().Height;

        io.DisplaySize = ImVec2((float)width, (float)height);

        // Rendering
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    /** Called from the message loop to notify user has entered a character. */
    void D3D11ImGuiAPI::CharInput(UINT32 character)
    { }

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
 
    void D3D11ImGuiAPI::CursorDoubleClick(const Vector2I& cursorPos, const OSPointerButtonStates& btnStates)
    {

    }

    void D3D11ImGuiAPI::MouseWheelScrolled(float scrollPos)
    {
        ImGuiIO& io = ImGui::GetIO();

        io.MouseWheel = scrollPos;
    }

    void D3D11ImGuiAPI::ButtonUp(ButtonEvent event)
    {
        /*if (event.isKeyboard())
        {
            ImGuiIO& io = ImGui::GetIO();
            io.KeysDown[event.buttonCode] = false;
        }*/
    }

    void D3D11ImGuiAPI::ButtonDown(ButtonEvent event)
    {
        /*if (event.isKeyboard())
        {
            ImGuiIO& io = ImGui::GetIO();
            io.KeysDown[event.buttonCode] = true;
        }*/
    }

    void D3D11ImGuiAPI::OnInputCommandEntered(InputCommandType commandType)
    {
        TE_PRINT("command");
    }

    void D3D11ImGuiAPI::KeyUp(UINT32 keyCode)
    { 
        ImGuiIO& io = ImGui::GetIO();
        io.KeysDown[keyCode] = false;

        TE_PRINT("key up");
    }

    void D3D11ImGuiAPI::KeyDown(UINT32 keyCode)
    { 
        ImGuiIO& io = ImGui::GetIO();
        io.KeysDown[keyCode] = true;

        TE_PRINT("key down");
    }
}
