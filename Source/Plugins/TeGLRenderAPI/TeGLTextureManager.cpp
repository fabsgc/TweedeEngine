#include "TeGLTextureManager.h"
#include "TeGLTexture.h"

namespace te
{
	PixelFormat GLTextureManager::GetNativeFormat(TextureType ttype, PixelFormat format, int usage, bool hwGamma)
	{
		return PF_R8; // TODO
	}

	SPtr<Texture> GLTextureManager::CreateTextureInternal(const TEXTURE_DESC& desc, const SPtr<PixelData>& initialData)
	{
        SPtr<GLTexture> texPtr = te_core_ptr<GLTexture>(new (te_allocate<GLTexture>()) GLTexture(desc, initialData));
		texPtr->SetThisPtr(texPtr);

		return texPtr;
	}
}
