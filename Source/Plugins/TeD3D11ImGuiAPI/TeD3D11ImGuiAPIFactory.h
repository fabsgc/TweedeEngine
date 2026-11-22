#pragma once

#include "TeD3D11ImGuiAPIPrerequisites.h"
#include "Gui/TeGuiAPIFactory.h"

namespace te
{
    class D3D11ImGuiAPIFactory : public GuiAPIFactory
    {
    public:
        static constexpr const char* SystemName = "TeD3D11ImGuiAPI";

        /** @copydoc GuiAPIFactory::Create */
        SPtr<GuiAPI> Create() override;

        /** @copydoc GuiAPIFactory::Destroy */
        void Destroy() override;

        /** @copydoc GuiAPIFactory::Name */
        const String& Name() const override;
    };
}