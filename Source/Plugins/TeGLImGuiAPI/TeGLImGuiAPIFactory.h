#pragma once

#include "TeGLImGuiAPIPrerequisites.h"
#include "Gui/TeGuiAPIFactory.h"

namespace te
{
    class GLImGuiAPIFactory : public GuiAPIFactory
    {
    public:
        static constexpr const char* SystemName = "TeGLImGuiAPI";

        /** @copydoc GuiAPIFactory::Destroy */
        SPtr<GuiAPI> Create() override;

        /** @copydoc GuiAPIFactory::Destroy */
        void Destroy() override;

        /** @copydoc GuiAPIFactory::Name */
        const String& Name() const override;
    };
}