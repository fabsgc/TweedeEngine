#include "Manager/TeRenderDocManager.h"

#include "Utility/TeDynLib.h"
#include "Utility/TeDynLibManager.h"
#include "RenderDoc/app/renderdoc_app.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(RenderDocManager)

    RenderDocManager::RenderDocManager()
    { }

    void RenderDocManager::OnStartUp()
    {
#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
        _rdcLib = gDynLibManager().Load(RENDERDOC_PATH);

        if (_rdcLib != nullptr && _rdcLib->IsLoaded())
        {
#if TE_PLATFORM == TE_PLATFORM_WIN32
            pRENDERDOC_GetAPI rdcGetAPI = nullptr;
            rdcGetAPI = (pRENDERDOC_GetAPI)_rdcLib->GetSymbol("RENDERDOC_GetAPI");

            TE_ASSERT_ERROR(rdcGetAPI != nullptr, "Failed to RENDERDOC_GetAPI function address from renderdoc.dll");
            TE_ASSERT_ERROR(rdcGetAPI(eRENDERDOC_API_Version_1_6_0, (void**)&_rdcAPI) != 0, "Failed to get RenderDoc API pointer");
#endif
        }

        if (_rdcAPI)
        {
            _rdcAPI->SetCaptureOptionU32(eRENDERDOC_Option_DebugOutputMute, 0);
            _rdcAPI->SetCaptureOptionU32(eRENDERDOC_Option_APIValidation, 1);
            _rdcAPI->SetCaptureOptionU32(eRENDERDOC_Option_CaptureCallstacks, 1);
            _rdcAPI->SetCaptureOptionU32(eRENDERDOC_Option_VerifyBufferAccess, 1);
            _rdcAPI->SetCaptureOptionU32(eRENDERDOC_Option_RefAllResources, 1);

            _rdcAPI->MaskOverlayBits(eRENDERDOC_Overlay_None, eRENDERDOC_Overlay_None);
        }
        else
        {
            TE_DEBUG("RenderDoc API has not been initialised");
        }
#else
        return;
#endif
    }

    void RenderDocManager::OnShutDown()
    {
        if (_rdcLib != nullptr && _rdcLib->IsLoaded())
            gDynLibManager().Unload(_rdcLib);
    }

    void RenderDocManager::FrameCapture()
    {
#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
        if (_rdcAPI == nullptr)
            return;

        _rdcAPI->TriggerCapture();

        // If the RenderDoc UI is already running, make sure it's visible.
        if (_rdcAPI->IsTargetControlConnected())
        {
            TE_DEBUG("Bringing RenderDoc to foreground...");
            _rdcAPI->ShowReplayUI();
            return;
        }

        // If the RenderDoc UI is not running, launch it and connect.
        TE_DEBUG("Launching RenderDoc...");
        if (_rdcAPI->LaunchReplayUI(true, "") == 0)
        {
            TE_DEBUG("Failed to launch RenderDoc");
        }
#endif
    }
}
