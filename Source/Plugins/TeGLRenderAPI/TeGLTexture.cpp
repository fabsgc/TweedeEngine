#include "TeGLTexture.h"
#include "TeGLRenderAPI.h"

namespace te
{
    GLTexture::GLTexture(const TEXTURE_DESC& desc, const SPtr<PixelData>& initialData)
        : Texture(desc, initialData)
    { }

    GLTexture::~GLTexture()
    {
        // TODO
    }

    void GLTexture::Initialize()
    {
        // TODO
        Texture::Initialize();
    }

    PixelData GLTexture::LockImpl(GpuLockOptions options, UINT32 mipLevel, UINT32 face, UINT32 deviceIdx, UINT32 queueIdx)
    {
        // TODO
        return PixelData();
    }

    void GLTexture::UnlockImpl()
    {
        // TODO
    }

    void GLTexture::CopyImpl(const SPtr<Texture>& target, const TEXTURE_COPY_DESC& desc)
    {
        // TODO
    }

    void GLTexture::ReadDataImpl(PixelData& dest, UINT32 mipLevel, UINT32 face, UINT32 deviceIdx, UINT32 queueIdx)
    {
        // TODO
    }

    void GLTexture::WriteDataImpl(const PixelData& src, UINT32 mipLevel, UINT32 face, bool discardWholeBuffer, UINT32 queueIdx)
    {
        // TODO
    }
}
