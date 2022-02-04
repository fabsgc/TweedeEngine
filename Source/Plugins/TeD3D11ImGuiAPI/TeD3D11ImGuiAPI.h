#pragma once

#include "TeD3D11ImGuiAPIPrerequisites.h"
#include "Gui/TeGuiAPI.h"

namespace te
{
    class D3D11ImGuiAPI: public GuiAPI
    {
    public:
        D3D11ImGuiAPI();
        virtual ~D3D11ImGuiAPI();

        TE_MODULE_STATIC_HEADER_MEMBER(D3D11ImGuiAPI)

        /** @copydoc GuiAPI::Initialize */
        void Initialize(void* data) override;

        /** @copydoc GuiAPI::Destroy */
        void Destroy() override;

        /** @copydoc GuiAPI::Update */
        void Update() override;

        /** @copydoc GuiAPI::BeginFrame */
        void BeginFrame() override;

        /** @copydoc GuiAPI::EndFrame */
        void EndFrame() override;

        /** @copydoc GuiAPI::HasFocus */
        bool HasFocus(FocusType type) override;

    public:
        /** @copydoc GuiAPI::CharInput */
        void CharInput(UINT32 character) override;

        /** @copydoc GuiAPI::CursorMoved */
        void CursorMoved(const Vector2I& cursorPos, const OSPointerButtonStates& btnStates) override;

        /** @copydoc GuiAPI::CursorPressed */
        void CursorPressed(const Vector2I& cursorPos, OSMouseButton button, const OSPointerButtonStates& btnStates) override;

        /** @copydoc GuiAPI::CursorReleased */
        void CursorReleased(const Vector2I& cursorPos, OSMouseButton button, const OSPointerButtonStates& btnStates) override;

        /** @copydoc GuiAPI::CursorDoubleClick */
        virtual void CursorDoubleClick(const Vector2I& cursorPos, const OSPointerButtonStates& btnStates) override;

        /** @copydoc GuiAPI::MouseWheelScrolled */
        void MouseWheelScrolled(float scrollPos) override;

        /** @copydoc GuiAPI::KeyUp */
        void KeyUp(UINT32 keyCode) override;

        /** @copydoc GuiAPI::KeyDown */
        void KeyDown(UINT32 keyCode) override;
    };
}
