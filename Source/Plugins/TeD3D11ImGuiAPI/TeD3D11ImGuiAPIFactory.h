#pragma once

#include "TeD3D11ImGuiAPIPrerequisites.h"
#include "Gui/TeGuiAPIFactory.h"

namespace te
{
    class D3D11ImGuiAPIFactory : public GuiAPIFactory
    {
    public:
        static constexpr const char* SystemName = "TeD3D11ImGuiAPI";

        SPtr<GuiAPI> Create() override;

        const String& Name() const override;
    };
}