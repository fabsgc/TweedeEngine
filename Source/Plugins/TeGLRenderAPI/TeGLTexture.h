#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "Image/TeTexture.h"
#include "TeGLSupport.h"

namespace te
{
    /** OpenGL implementation of a texture. */
    class GLTexture : public Texture
    {
    public:
        virtual ~GLTexture();

        /**	Returns OpenGL texture target type. */
        GLenum GetGLTextureTarget() const;

        /**	Returns internal OpenGL texture handle. */
        GLuint GetGLID() const;

        /**	Returns the internal OpenGL format used by the texture. */
        GLenum GetGLFormat() const { return _GLFormat; }

        /**
         * Returns a hardware pixel buffer for a certain face and level of the texture.
         *
         * @param[in]	face	Index of the texture face, if texture has more than one. Array index for texture arrays and
         *						a cube face for cube textures.
         * @param[in]	mipmap	Index of the mip level. 0 being the largest mip level.
         *
         * @note	Cube face indices: +X (0), -X (1), +Y (2), -Y (3), +Z (4), -Z (5)
         */
        //SPtr<GLPixelBuffer> GetBuffer(UINT32 face, UINT32 mipmap); TODO

        /**
         * Picks an OpenGL texture target based on the texture type, number of samples per pixel, and number of faces.
         */
        static GLenum GetGLTextureTarget(TextureType type, UINT32 numSamples, UINT32 numFaces);

        /** Picks an OpenGL texture target based on a GPU program parameter type. */
        static GLenum GetGLTextureTarget(GpuParamObjectType type);

    protected:
        friend class GLTextureManager;
        friend class TextureView;

        GLTexture(GLSupport& support, const TEXTURE_DESC& desc, const SPtr<PixelData>& initialData);

        /** @copydoc CoreObject::Initialize */
        void Initialize() override;

        /** @copydoc Texture::LockImpl */
        PixelData LockImpl(GpuLockOptions options, UINT32 mipLevel = 0, UINT32 face = 0, UINT32 deviceIdx = 0, UINT32 queueIdx = 0) override;

        /** @copydoc Texture::UnlockImpl */
        void UnlockImpl() override;

        /** @copydoc Texture::CopyImpl */
        void CopyImpl(const SPtr<Texture>& target, const TEXTURE_COPY_DESC& desc) override;

        /** @copydoc Texture::ReadData */
        void ReadDataImpl(PixelData& dest, UINT32 mipLevel = 0, UINT32 face = 0, UINT32 deviceIdx = 0, UINT32 queueIdx = 0) override;

        /** @copydoc Texture::WriteData */
        void WriteDataImpl(const PixelData& src, UINT32 mipLevel = 0, UINT32 face = 0, bool discardWholeBuffer = false, UINT32 queueIdx = 0) override;

        /** Creates pixel buffers for each face and mip level. Texture must have been created previously. */
        void CreateSurfaceList();

        /**	Creates an empty and uninitialized texture view object. */
        SPtr<TextureView> CreateView(const TEXTURE_VIEW_DESC& desc) override;

    private:
        GLuint _textureID = 0;
        GLenum _GLFormat = 0;
        PixelFormat _internalFormat = PF_UNKNOWN;
        GLSupport& _GLSupport;
    };
}
