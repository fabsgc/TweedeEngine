#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"

namespace te
{
    class TE_CORE_EXPORT RenderAPI : public Module<RenderAPI>
    {
    public:
        RenderAPI();
        ~RenderAPI();

        TE_MODULE_STATIC_HEADER_MEMBER(RenderAPI)

        virtual SPtr<RenderWindow> CreateRenderWindow(const RENDER_WINDOW_DESC& windowDesc) = 0;

        virtual void Initialize() = 0;
        virtual void Update() = 0;
    };
}