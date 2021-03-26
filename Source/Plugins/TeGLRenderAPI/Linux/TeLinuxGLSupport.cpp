#include "Private/Linux/TeLinuxPlatform.h"
#include "Linux/TeLinuxGLSupport.h"
#include "Linux/TeLinuxContext.h"
#include "Linux/TeLinuxRenderWindow.h"
#include "Linux/TeLinuxVideoModeInfo.h"
#include "TeGLRenderAPI.h"

namespace te
{
    LinuxGLSupport::LinuxGLSupport()
    { }

    void LinuxGLSupport::Start()
    {
        // Retrieve all essential extensions
        LinuxPlatform::LockX();

        LinuxPlatform::UnlockX();
    }

    void LinuxGLSupport::Stop()
    {
        // Do nothing
    }

    SPtr<LinuxContext> LinuxGLSupport::CreateContext(::Display* x11display, XVisualInfo& visualInfo)
    {
        GLRenderAPI* rapi = static_cast<GLRenderAPI*>(RenderAPI::instancePtr());

        // If RenderAPI has initialized a context use that, otherwise we create our own
        if (!rapi->IsContextInitialized())
            return te_shared_ptr_new<LinuxContext>(x11display, visualInfo);
        else
        {
            SPtr<GLContext> context = rapi->GetMainContext();
            return std::static_pointer_cast<LinuxContext>(context);
        }
    }

    void* LinuxGLSupport::GetProcAddress(const String& procname)
    {
        return (void*)glXGetProcAddressARB((const GLubyte*)procname.c_str());
    }

    SPtr<VideoModeInfo> LinuxGLSupport::GetVideoModeInfo() const
    {
        return te_shared_ptr_new<LinuxVideoModeInfo>();
    }
}
