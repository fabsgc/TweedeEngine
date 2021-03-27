#include "TeGLHardwareBuffer.h"
#include "TeGLHardwareBufferManager.h"

namespace te
{
    GLHardwareBuffer::GLHardwareBuffer(GLenum target, UINT32 size, GpuBufferUsage usage)
        : HardwareBuffer(size, usage, GDF_DEFAULT)
        , _target(target)
    { 
        glGenBuffers(1, &_bufferId);
        TE_CHECK_GL_ERROR();

        if (!_bufferId)
            TE_ASSERT_ERROR(false, "Cannot create GL buffer");

        glBindBuffer(target, _bufferId);
        TE_CHECK_GL_ERROR();

        glBufferData(target, size, nullptr, GLHardwareBufferManager::GetGLUsage(usage));
        TE_CHECK_GL_ERROR();
    }

    GLHardwareBuffer::~GLHardwareBuffer()
    { 
        if (_bufferId != 0)
        {
            glDeleteBuffers(1, &_bufferId);
            TE_CHECK_GL_ERROR();
        }
    }

    void* GLHardwareBuffer::Map(UINT32 offset, UINT32 length, GpuLockOptions options, UINT32 deviceIdx, UINT32 queueIdx)
    {
        // If no buffer ID it's assumed this type of buffer is unsupported and we silently fail (it's up to the creator
        // if the buffer to check for support and potentially print a warning)
        if(_bufferId == 0)
            return nullptr;

        GLenum access = 0;

        glBindBuffer(_target, _bufferId);
        TE_CHECK_GL_ERROR();

        if ((options == GBL_WRITE_ONLY) || (options == GBL_WRITE_ONLY_NO_OVERWRITE) || (options == GBL_WRITE_ONLY_DISCARD))
        {
            access = GL_MAP_WRITE_BIT;

            if (options == GBL_WRITE_ONLY_DISCARD)
                access |= GL_MAP_INVALIDATE_BUFFER_BIT;
            else if (options == GBL_WRITE_ONLY_NO_OVERWRITE)
                access |= GL_MAP_UNSYNCHRONIZED_BIT;
        }
        else if (options == GBL_READ_ONLY)
            access = GL_MAP_READ_BIT;
        else
            access = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;

        void* buffer = nullptr;

        if (length > 0)
        {
            buffer = glMapBufferRange(_target, offset, length, access);
            TE_CHECK_GL_ERROR();

            if (buffer == nullptr)
                TE_ASSERT_ERROR(false, "Cannot map OpenGL buffer.");

            _zeroLocked = false;
        }
        else
            _zeroLocked = true;

        return static_cast<void*>(static_cast<unsigned char*>(buffer));
    }

    void GLHardwareBuffer::Unmap()
    {
        if(_bufferId == 0)
            return;

        glBindBuffer(_target, _bufferId);
        TE_CHECK_GL_ERROR();

        if (!_zeroLocked)
        {
            if (!glUnmapBuffer(_target))
            {
                TE_CHECK_GL_ERROR();
                TE_ASSERT_ERROR(false, "Buffer data corrupted, please reload.");
            }
        }
    }

    void GLHardwareBuffer::CopyData(HardwareBuffer& srcBuffer, UINT32 srcOffset,
        UINT32 dstOffset, UINT32 length, bool discardWholeBuffer)
    {
        if(_bufferId == 0)
            return;

        GLHardwareBuffer& glSrcBuffer = static_cast<GLHardwareBuffer&>(srcBuffer);

        glBindBuffer(GL_COPY_READ_BUFFER, glSrcBuffer.GetGLBufferId());
        TE_CHECK_GL_ERROR();

        glBindBuffer(GL_COPY_WRITE_BUFFER, _bufferId);
        TE_CHECK_GL_ERROR();

        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, srcOffset, dstOffset, length);
        TE_CHECK_GL_ERROR();
    }

    void GLHardwareBuffer::ReadData(UINT32 offset, UINT32 length, void* dest, UINT32 deviceIdx, UINT32 queueIdx)
    {
        if(_bufferId == 0)
            return;

        void* bufferData = Lock(offset, length, GBL_READ_ONLY, deviceIdx, queueIdx);
        memcpy(dest, bufferData, length);
        Unlock();
    }

    void GLHardwareBuffer::WriteData(UINT32 offset, UINT32 length, const void* pSource, BufferWriteType writeFlags,
        UINT32 queueIdx)
    {
        if(_bufferId == 0)
            return;

        GpuLockOptions lockOption = GBL_WRITE_ONLY;
        if (writeFlags == BWT_DISCARD)
            lockOption = GBL_WRITE_ONLY_DISCARD;
        else if (writeFlags == BTW_NO_OVERWRITE)
            lockOption = GBL_WRITE_ONLY_NO_OVERWRITE;

        void* bufferData = Lock(offset, length, lockOption, 0, queueIdx);
        memcpy(bufferData, pSource, length);
        Unlock();
    }
}
