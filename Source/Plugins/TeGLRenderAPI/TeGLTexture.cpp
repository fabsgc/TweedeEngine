#include "TeGLTexture.h"
#include "TeGLRenderAPI.h"
#include "TeGLPixelFormat.h"
#include "Utility/TeBitwise.h"
#include "Image/TeTextureManager.h"
#include "TeGLRenderTexture.h"

namespace te
{
    GLTexture::GLTexture(GLSupport& support, const TEXTURE_DESC& desc, const SPtr<PixelData>& initialData)
        : Texture(desc, initialData)
        , _GLSupport(support)
    { }

    GLTexture::~GLTexture()
    {
        glDeleteTextures(1, &_textureID);
        TE_CHECK_GL_ERROR();

        ClearBufferViews();

    }

    void GLTexture::Initialize()
    {
        UINT32 width = _properties.GetWidth();
        UINT32 height = _properties.GetHeight();
        UINT32 depth = _properties.GetDepth();
        TextureType texType = _properties.GetTextureType();
        int usage = _properties.GetUsage();
        UINT32 numMips = _properties.GetNumMipmaps();
        UINT32 numFaces = _properties.GetNumFaces();

        // 0-sized textures aren't supported by the API
        width = std::max(width, 1U);
        height = std::max(height, 1U);

        PixelFormat pixFormat = _properties.GetFormat();
        _internalFormat = GLPixelUtil::GetClosestSupportedPF(pixFormat, texType, usage);

        if (pixFormat != _internalFormat)
        {
            TE_DEBUG("Provided pixel format is not supported by the driver: " + ToString(pixFormat) + ". " + 
                "Falling back on: " + ToString(_internalFormat) + ".");
        }

        // Check requested number of mipmaps
        UINT32 maxMips = PixelUtil::GetMaxMipmaps(width, height, depth);
        if (numMips > maxMips)
        {
            TE_DEBUG("Invalid number of mipmaps. Maximum allowed is: " + ToString(maxMips));
            numMips = maxMips;
        }

        if ((usage & TU_DEPTHSTENCIL) != 0)
        {
            if (texType != TEX_TYPE_2D && texType != TEX_TYPE_CUBE_MAP)
            {
                TE_DEBUG("Only 2D and cubemap depth stencil textures are supported. Ignoring depth-stencil flag.");
                usage &= ~TU_DEPTHSTENCIL;
            }
        }

        // Include the base mip level
        numMips += 1;

        // Generate texture handle
        glGenTextures(1, &_textureID);
        TE_CHECK_GL_ERROR();

        // Set texture type
        glBindTexture(GetGLTextureTarget(), _textureID);
        TE_CHECK_GL_ERROR();

        if (_properties.GetNumSamples() <= 1)
        {
            // This needs to be set otherwise the texture doesn't get rendered
            glTexParameteri(GetGLTextureTarget(), GL_TEXTURE_MAX_LEVEL, numMips - 1);
            TE_CHECK_GL_ERROR();
        }

        // Allocate internal buffer so that glTexSubImageXD can be used
        _GLFormat = GLPixelUtil::GetGLInternalFormat(_internalFormat, _properties.IsHardwareGammaEnabled());

        UINT32 sampleCount = _properties.GetNumSamples();
        if ((usage & (TU_RENDERTARGET | TU_DEPTHSTENCIL)) != 0 && _properties.GetTextureType() == TEX_TYPE_2D && sampleCount > 1)
        {
            if (numFaces <= 1)
            {
                // Create immutable storage if available, fallback to mutable
#if TE_OPENGL_4_3 || TE_OPENGLES_3_1
                glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, sampleCount, _GLFormat, width, height, GL_TRUE);
                TE_CHECK_GL_ERROR();
#else
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, sampleCount, _GLFormat, width, height, GL_TRUE);
                TE_CHECK_GL_ERROR();
#endif
            }
            else
            {
                // Create immutable storage if available, fallback to mutable
#if TE_OPENGL_4_3 || TE_OPENGLES_3_2
                glTexStorage3DMultisample(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, sampleCount, _GLFormat, width, height, numFaces, GL_TRUE);
                TE_CHECK_GL_ERROR();
#else
                glTexImage3DMultisample(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, sampleCount, _GLFormat, width, height, numFaces, GL_TRUE);
                TE_CHECK_GL_ERROR();
#endif
            }
        }
        else
        {
            // Create immutable storage if available, fallback to mutable
#if TE_OPENGL_4_2 || TE_OPENGLES_3_1
            switch (texType)
            {
            case TEX_TYPE_1D:
            {
                if (numFaces <= 1)
                {
                    glTexStorage1D(GL_TEXTURE_1D, numMips, _GLFormat, width);
                    TE_CHECK_GL_ERROR();
                }
                else
                {
                    glTexStorage2D(GL_TEXTURE_1D_ARRAY, numMips, _GLFormat, width, numFaces);
                    TE_CHECK_GL_ERROR();
                }
            }
            break;
            case TEX_TYPE_2D:
            {
                if (numFaces <= 1)
                {
                    glTexStorage2D(GL_TEXTURE_2D, numMips, _GLFormat, width, height);
                    TE_CHECK_GL_ERROR();
                }
                else
                {
                    glTexStorage3D(GL_TEXTURE_2D_ARRAY, numMips, _GLFormat, width, height, numFaces);
                    TE_CHECK_GL_ERROR();
                }
            }
            break;
            case TEX_TYPE_3D:
                glTexStorage3D(GL_TEXTURE_3D, numMips, _GLFormat, width, height, depth);
                TE_CHECK_GL_ERROR();
                break;
            case TEX_TYPE_CUBE_MAP:
            {
                if (numFaces <= 6)
                {
                    glTexStorage2D(GL_TEXTURE_CUBE_MAP, numMips, _GLFormat, width, height);
                    TE_CHECK_GL_ERROR();
                }
                else
                {
                    glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, numMips, _GLFormat, width, height, numFaces);
                    TE_CHECK_GL_ERROR();
                }
            }
            break;
            }
#else
            if ((usage & TU_DEPTHSTENCIL) != 0)
            {
                GLenum depthStencilType = GLPixelUtil::GetDepthStencilTypeFromPF(_internalFormat);
                GLenum depthStencilFormat = GLPixelUtil::GetDepthStencilFormatFromPF(_internalFormat);

                if (texType == TEX_TYPE_2D)
                {
                    if (numFaces <= 1)
                    {
                        glTexImage2D(GL_TEXTURE_2D, 0, _GLFormat, width, height, 0,
                            depthStencilFormat, depthStencilType, nullptr);
                        TE_CHECK_GL_ERROR();
                    }
                    else
                    {
                        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, _GLFormat, width, height, numFaces, 0,
                            depthStencilFormat, depthStencilType, nullptr);
                        TE_CHECK_GL_ERROR();
                    }
                }
                else if (texType == TEX_TYPE_CUBE_MAP)
                {
                    if (numFaces <= 6)
                    {
                        for (UINT32 face = 0; face < 6; face++)
                        {
                            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, _GLFormat,
                                width, height, 0, depthStencilFormat, depthStencilType, nullptr);
                            TE_CHECK_GL_ERROR();
                        }
                    }
                    else
                    {
                        glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, _GLFormat,
                            width, height, numFaces, 0, depthStencilFormat, depthStencilType, nullptr);
                        TE_CHECK_GL_ERROR();
                    }
                }
                else
                {
                    TE_DEBUG("Unsupported texture type for depth-stencil attachment usage.");
                }
            }
            else
            {
                GLenum baseFormat = GLPixelUtil::GetGLOriginFormat(_internalFormat);
                GLenum baseDataType = GLPixelUtil::GetGLOriginDataType(_internalFormat);

                for (UINT32 mip = 0; mip < numMips; mip++)
                {
                    switch (texType)
                    {
                    case TEX_TYPE_1D:
                    {
                        if (numFaces <= 1)
                        {
                            glTexImage1D(GL_TEXTURE_1D, mip, _GLFormat, width, 0, baseFormat, baseDataType, nullptr);
                            TE_CHECK_GL_ERROR();
                        }
                        else
                        {
                            glTexImage2D(GL_TEXTURE_1D_ARRAY, mip, _GLFormat, width, numFaces, 0, baseFormat, baseDataType, nullptr);
                            TE_CHECK_GL_ERROR();
                        }
                    }
                    break;
                    case TEX_TYPE_2D:
                    {
                        if (numFaces <= 1)
                        {
                            glTexImage2D(GL_TEXTURE_2D, mip, _GLFormat, width, height, 0, baseFormat, baseDataType, nullptr);
                            TE_CHECK_GL_ERROR();
                        }
                        else
                        {
                            glTexImage3D(GL_TEXTURE_2D_ARRAY, mip, _GLFormat, width, height, numFaces, 0, baseFormat, baseDataType, nullptr);
                            TE_CHECK_GL_ERROR();
                        }
                    }
                    break;
                    case TEX_TYPE_3D:
                        glTexImage3D(GL_TEXTURE_3D, mip, _GLFormat, width, height,
                            depth, 0, baseFormat, baseDataType, nullptr);
                        TE_CHECK_GL_ERROR();
                        break;
                    case TEX_TYPE_CUBE_MAP:
                    {
                        if (numFaces <= 6)
                        {
                            for (UINT32 face = 0; face < 6; face++)
                            {
                                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, mip, _GLFormat,
                                    width, height, 0, baseFormat, baseDataType, nullptr);
                                TE_CHECK_GL_ERROR();
                            }
                        }
                        else
                        {
                            glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, mip, _GLFormat,
                                width, height, numFaces, 0, baseFormat, baseDataType, nullptr);
                            TE_CHECK_GL_ERROR();
                        }
                    }
                    break;
                    }

                    if (width > 1)
                        width = width / 2;

                    if (height > 1)
                        height = height / 2;

                    if (depth > 1)
                        depth = depth / 2;
                }
            }
#endif
        }

        CreateSurfaceList();

        Texture::Initialize();
    }

    GLenum GLTexture::GetGLTextureTarget() const
    {
        return GetGLTextureTarget(_properties.GetTextureType(), _properties.GetNumSamples(), _properties.GetNumFaces());
    }

    GLuint GLTexture::GetGLID() const
    {
        return _textureID;
    }

    GLenum GLTexture::GetGLTextureTarget(TextureType type, UINT32 numSamples, UINT32 numFaces)
    {
        switch (type)
        {
        case TEX_TYPE_1D:
            if (numFaces <= 1)
                return GL_TEXTURE_1D;
            else
                return GL_TEXTURE_1D_ARRAY;
        case TEX_TYPE_2D:
            if (numSamples > 1)
            {
                if (numFaces <= 1)
                    return GL_TEXTURE_2D_MULTISAMPLE;
                else
                    return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
            }
            else
            {
                if (numFaces <= 1)
                    return GL_TEXTURE_2D;
                else
                    return GL_TEXTURE_2D_ARRAY;
            }
        case TEX_TYPE_3D:
            return GL_TEXTURE_3D;
        case TEX_TYPE_CUBE_MAP:
            if (numFaces <= 6)
                return GL_TEXTURE_CUBE_MAP;
            else
                return GL_TEXTURE_CUBE_MAP_ARRAY;
        default:
            return 0;
        };
    }

    GLenum GLTexture::GetGLTextureTarget(GpuParamObjectType type)
    {
        switch (type)
        {
        case GPOT_TEXTURE1D:
            return GL_TEXTURE_1D;
        case GPOT_TEXTURE2D:
            return GL_TEXTURE_2D;
        case GPOT_TEXTURE2DMS:
            return GL_TEXTURE_2D_MULTISAMPLE;
        case GPOT_TEXTURE3D:
            return GL_TEXTURE_3D;
        case GPOT_TEXTURECUBE:
            return GL_TEXTURE_CUBE_MAP;
        case GPOT_TEXTURE1DARRAY:
            return GL_TEXTURE_1D_ARRAY;
        case GPOT_TEXTURE2DARRAY:
            return GL_TEXTURE_2D_ARRAY;
        case GPOT_TEXTURE2DMSARRAY:
            return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
        case GPOT_TEXTURECUBEARRAY:
            return GL_TEXTURE_CUBE_MAP_ARRAY;
        default:
            return GL_TEXTURE_2D;
        }
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

    void GLTexture::CreateSurfaceList()
    {
        // TODO
    }

    SPtr<TextureView> GLTexture::CreateView(const TEXTURE_VIEW_DESC& desc)
    {
        return nullptr;
    }
}
