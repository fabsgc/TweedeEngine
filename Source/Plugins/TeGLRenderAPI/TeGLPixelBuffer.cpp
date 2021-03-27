#include "TeGLPixelBuffer.h"
#include "TeGLTexture.h"
#include "TeGLSupport.h"
#include "TeGLPixelFormat.h"
#include "Utility/TeBitwise.h"
#include "TeGLRenderTexture.h"
#include "Math/TeMath.h"

namespace te
{
    GLPixelBuffer::GLPixelBuffer(UINT32 inWidth, UINT32 inHeight, UINT32 inDepth, PixelFormat inFormat, GpuBufferUsage usage)
        : _usage(usage)
        , _width(inWidth)
        , _height(inHeight)
        , _depth(inDepth)
        , _format(inFormat)
        , _buffer(inWidth, inHeight, inDepth, inFormat)
    {
        _sizeInBytes = _height * _width * PixelUtil::GetNumElemBytes(_format);
        _currentLockOptions = (GpuLockOptions)0;
    }

    GLPixelBuffer::~GLPixelBuffer()
    {
        _buffer.FreeInternalBuffer();
    }

    void GLPixelBuffer::AllocateBuffer()
    {
        if (_buffer.GetData())
            return;

        _buffer.AllocateInternalBuffer();
    }

    void GLPixelBuffer::FreeBuffer()
    {
        if (_usage & GBU_STATIC)
            _buffer.FreeInternalBuffer();
    }

    void* GLPixelBuffer::Lock(UINT32 offset, UINT32 length, GpuLockOptions options)
    {
        assert(!_isLocked && "Cannot lock this buffer, it is already locked!");
        assert(offset == 0 && length == _sizeInBytes && "Cannot lock memory region, most lock box or entire buffer");

        PixelVolume volume(0, 0, 0, _width, _height, _depth);
        const PixelData& lockedData = Lock(volume, options);
        return lockedData.GetData();
    }

    const PixelData& GLPixelBuffer::Lock(const PixelVolume& lockBox, GpuLockOptions options)
    {
        AllocateBuffer();

        if (options != GBL_WRITE_ONLY_DISCARD)
        {
            // Download the old contents of the texture
            Download(_buffer);
        }

        _currentLockOptions = options;
        _lockedBox = lockBox;

        _currentLock = _buffer.GetSubVolume(lockBox);
        _isLocked = true;

        return _currentLock;
    }

    void GLPixelBuffer::Unlock()
    {
        assert(_isLocked && "Cannot unlock this buffer, it is not locked!");

        if (_currentLockOptions != GBL_READ_ONLY)
        {
            // From buffer to card, only upload if was locked for writing
            Upload(_currentLock, _lockedBox);
        }

        FreeBuffer();
        _isLocked = false;
    }

    void GLPixelBuffer::Upload(const PixelData& data, const PixelVolume& dest)
    {
        TE_ASSERT_ERROR(false, "Upload not possible for this pixel buffer type");
    }

    void GLPixelBuffer::Download(const PixelData& data)
    {
        TE_ASSERT_ERROR(false, "Download not possible for this pixel buffer type");
    }

    void GLPixelBuffer::BlitFromTexture(GLTextureBuffer* src)
    {
        BlitFromTexture(src,
            PixelVolume(0, 0, 0, src->GetWidth(), src->GetHeight(), src->GetDepth()),
            PixelVolume(0, 0, 0, _width, _height, _depth)
        );
    }

    void GLPixelBuffer::BlitFromTexture(GLTextureBuffer* src, const PixelVolume& srcBox, const PixelVolume& dstBox)
    {
        TE_ASSERT_ERROR(false, "BlitFromTexture not possible for this pixel buffer type");
    }

    void GLPixelBuffer::BindToFramebuffer(GLenum attachment, UINT32 zoffset, bool allLayers)
    {
        TE_ASSERT_ERROR(false, "Framebuffer bind not possible for this pixel buffer type");
    }

    GLTextureBuffer::GLTextureBuffer(GLenum target, GLuint id, GLint face, GLint level, PixelFormat format,
        GpuBufferUsage usage, bool hwGamma, UINT32 multisampleCount)
        : GLPixelBuffer(0, 0, 0, format, usage)
        , _target(target)
        , _textureID(id)
        , _face(face)
        , _level(level)
        , _multisampleCount(multisampleCount)
        , _HwGamma(hwGamma)
    {
        GLint value = 0;

        glBindTexture(_target, _textureID);
        TE_CHECK_GL_ERROR();

        // Get face identifier
        _faceTarget = _target;
        if (_target == GL_TEXTURE_CUBE_MAP)
            _faceTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X + (face % 6);

        // Get width
        glGetTexLevelParameteriv(_faceTarget, level, GL_TEXTURE_WIDTH, &value);
        TE_CHECK_GL_ERROR();

        _width = value;

        // Get height
        if (target == GL_TEXTURE_1D)
            value = 1;	// Height always 1 for 1D textures
        else
        {
            glGetTexLevelParameteriv(_faceTarget, level, GL_TEXTURE_HEIGHT, &value);
            TE_CHECK_GL_ERROR();
        }

        _height = value;

        // Get depth
        if (target != GL_TEXTURE_3D)
            value = 1; // Depth always 1 for non-3D textures
        else
        {
            glGetTexLevelParameteriv(_faceTarget, level, GL_TEXTURE_DEPTH, &value);
            TE_CHECK_GL_ERROR();
        }

        _depth = value;

        // Default
        _sizeInBytes = PixelUtil::GetMemorySize(_width, _height, _depth, _format);

        // Set up pixel box
        _buffer = PixelData(_width, _height, _depth, _format);
    }

    void GLTextureBuffer::Upload(const PixelData& data, const PixelVolume& dest)
    {
        // TODO
    }

    void GLTextureBuffer::Download(const PixelData& data)
    {
        // TODO
    }

    void GLTextureBuffer::BindToFramebuffer(GLenum attachment, UINT32 zoffset, bool allLayers)
    {
        // TODO
    }

    void GLTextureBuffer::CopyFromFramebuffer(UINT32 zoffset)
    {
        // TODO
    }

    void GLTextureBuffer::BlitFromTexture(GLTextureBuffer* src)
    {
        GLPixelBuffer::BlitFromTexture(src);
    }

    void GLTextureBuffer::BlitFromTexture(GLTextureBuffer* src, const PixelVolume& srcBox, const PixelVolume& dstBox)
    {
        // TODO
    }
}
