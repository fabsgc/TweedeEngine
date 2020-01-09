#include "TeRenderTarget.h"

namespace te
{
    void RenderTarget::GetCustomAttribute(const String& name, void* pData) const
    {
        TE_ASSERT_ERROR(false, "Attribute not found.", __FILE__, __LINE__);
    }
}
