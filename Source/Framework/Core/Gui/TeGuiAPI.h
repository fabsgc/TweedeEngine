#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"

namespace te
{
    class TE_CORE_EXPORT GuiAPI : public Module<GuiAPI>
    {
    public:
        GuiAPI() = default;
        ~GuiAPI() = default;

        TE_MODULE_STATIC_HEADER_MEMBER(GuiAPI)

        virtual void Initialize() {};
        virtual void Destroy() {};
        virtual void Update() {};
    };
}