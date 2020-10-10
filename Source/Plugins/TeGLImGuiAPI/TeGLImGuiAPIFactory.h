#pragma once

#include "TeGLImGuiAPIPrerequisites.h"
#include "Gui/TeGuiAPIFactory.h"

namespace te
{
    class GLImGuiAPIFactory : public GuiAPIFactory
    {
    public:
        static constexpr const char* SystemName = "TeGLImGuiAPI";

        SPtr<GuiAPI> Create() override;

        const String& Name() const override;
    };
}