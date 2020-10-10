#pragma once

#include "TeD3D11ImGuiAPIPrerequisites.h"
#include "Gui/TeGuiAPI.h"

namespace te
{
    class D3D11ImGuiAPI: public GuiAPI
    {
    public:
        D3D11ImGuiAPI();
        ~D3D11ImGuiAPI();

        /** @copydoc GuiAPI::Initialize */
        void Initialize() override;

        /** @copydoc GuiAPI::Destroy */
        void Destroy() override;

        /** @copydoc GuiAPI::Update */
        void Update() override;
    };
}
