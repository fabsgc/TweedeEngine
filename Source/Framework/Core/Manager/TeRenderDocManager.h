#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"

struct RENDERDOC_API_1_6_0;

namespace te
{
    /** Manager that handles gui render system start up. */
    class TE_CORE_EXPORT RenderDocManager : public Module<RenderDocManager>
    {
    public:
        RenderDocManager();
        virtual ~RenderDocManager() = default;

        TE_MODULE_STATIC_HEADER_MEMBER(RenderDocManager)

        void OnStartUp() override;
        void OnShutDown() override;

        void FrameCapture();

    private:
        DynLib* _rdcLib = nullptr;
        RENDERDOC_API_1_6_0* _rdcAPI = nullptr;
    };
}
