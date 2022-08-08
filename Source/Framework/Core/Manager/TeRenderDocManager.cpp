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
        _rdcLib = gDynLibManager().Load(RENDERDOC_PATH);

        if (_rdcLib != nullptr)
        {
#if TE_PLATFORM == TE_PLATFORM_WIN32
            pRENDERDOC_GetAPI rdcGetAPI = nullptr;
            rdcGetAPI = (pRENDERDOC_GetAPI)_rdcLib->GetSymbol("RENDERDOC_GetAPI");
#endif

            TE_ASSERT_ERROR(rdcGetAPI != nullptr, "Failed to RENDERDOC_GetAPI function address from renderdoc.dll");
            TE_ASSERT_ERROR(rdcGetAPI(eRENDERDOC_API_Version_1_5_0, (void**)&_rdcAPI) != 0, "Failed to get RenderDoc API pointer");
        }

        if(!_rdcAPI)
            TE_DEBUG("RenderDoc API has not been initialised");

        _rdcAPI->SetCaptureOptionU32(eRENDERDOC_Option_DebugOutputMute, 0);
        _rdcAPI->SetCaptureOptionU32(eRENDERDOC_Option_APIValidation, 1);
        _rdcAPI->SetCaptureOptionU32(eRENDERDOC_Option_CaptureCallstacks, 1);
        _rdcAPI->SetCaptureOptionU32(eRENDERDOC_Option_VerifyBufferAccess, 1);
        _rdcAPI->SetCaptureOptionU32(eRENDERDOC_Option_RefAllResources, 1);
        _rdcAPI->SetCaptureOptionU32(eRENDERDOC_Option_CaptureAllCmdLists, 1);

        _rdcAPI->MaskOverlayBits(eRENDERDOC_Overlay_None, eRENDERDOC_Overlay_None);
    }

    void RenderDocManager::OnShutDown()
    {
        if (_rdcLib != nullptr)
        {
            gDynLibManager().Unload(_rdcLib);
        }
    }

    void RenderDocManager::FrameCapture()
    {
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
    }
}
