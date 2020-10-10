#include "TeRenderTarget.h"

namespace te
{
    void RenderTarget::GetCustomAttribute(const String& name, void* pData) const
    {
        TE_ASSERT_ERROR(false, "Attribute not found.");
    }

    void RenderTarget::SetPriority(INT32 priority)
    {
        RenderTargetProperties& props = const_cast<RenderTargetProperties&>(GetProperties());

        props.Priority = priority;
    }
}
