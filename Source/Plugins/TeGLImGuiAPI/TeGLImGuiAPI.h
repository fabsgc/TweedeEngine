#pragma once

#include "TeGLImGuiAPIPrerequisites.h"
#include "Gui/TeGuiAPI.h"

namespace te
{
    class GLImGuiAPI: public GuiAPI
    {
    public:
        GLImGuiAPI();
        ~GLImGuiAPI();

        /** @copydoc GuiAPI::Initialize */
        void Initialize() override;

        /** @copydoc GuiAPI::Destroy */
        void Destroy() override;

        /** @copydoc GuiAPI::Update */
        void Update() override;
    };
}
