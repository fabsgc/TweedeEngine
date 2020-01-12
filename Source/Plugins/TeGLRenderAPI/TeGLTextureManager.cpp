#include "TeGLTextureManager.h"
#include "Image/TeTexture.h"

namespace te
{
	PixelFormat GLTextureManager::GetNativeFormat(TextureType ttype, PixelFormat format, int usage, bool hwGamma)
	{
		return PF_R8; // TODO
	}

	SPtr<Texture> GLTextureManager::CreateTextureInternal(const TEXTURE_DESC& desc, const SPtr<PixelData>& initialData)
	{
        SPtr<Texture> texPtr = te_core_ptr<Texture>(new (te_allocate<Texture>()) Texture()); // TODO
		texPtr->SetThisPtr(texPtr);

		return texPtr;
	}
}
