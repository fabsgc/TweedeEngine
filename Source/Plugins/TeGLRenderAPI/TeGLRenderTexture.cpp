#include "TeGLRenderTexture.h"

namespace te
{
    GLRenderTexture::GLRenderTexture(const RENDER_TEXTURE_DESC& desc, UINT32 deviceIdx)
		: RenderTexture(desc, deviceIdx)
        , _properties(desc, false)
	{}

	void D3D11RenderTexture::GetCustomAttribute(const String& name, void* data) const
	{
		return;
	}
}
