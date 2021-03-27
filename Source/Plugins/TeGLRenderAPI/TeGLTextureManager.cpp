#include "TeGLTextureManager.h"
#include "TeGLRenderTexture.h"
#include "TeGLTexture.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(GLTextureManager)

    GLTextureManager::GLTextureManager(GLSupport& support)
        : TextureManager()
        , _GLSupport(support)
    { }

    PixelFormat GLTextureManager::GetNativeFormat(TextureType ttype, PixelFormat format, int usage, bool hwGamma)
    {
        return PF_R8; // TODO
    }

    SPtr<Texture> GLTextureManager::CreateTextureInternal(const TEXTURE_DESC& desc, const SPtr<PixelData>& initialData)
    {
        SPtr<GLTexture> texPtr = te_core_ptr<GLTexture>(new (te_allocate<GLTexture>()) GLTexture(_GLSupport, desc, initialData));
        texPtr->SetThisPtr(texPtr);

        return texPtr;
    }

    SPtr<RenderTexture> GLTextureManager::CreateRenderTextureInternal(const RENDER_TEXTURE_DESC& desc, UINT32 deviceIdx)
    {
        SPtr<GLRenderTexture> texPtr = te_core_ptr<GLRenderTexture>(new (te_allocate<GLRenderTexture>()) GLRenderTexture(desc, deviceIdx));
        texPtr->SetThisPtr(texPtr);

        return texPtr;
    }
}
