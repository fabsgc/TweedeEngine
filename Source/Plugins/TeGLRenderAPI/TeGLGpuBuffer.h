#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "RenderAPI/TeGpuBuffer.h"
#include "TeGLHardwareBuffer.h"

namespace te
{
    /** OpenGL implementation of a generic GPU buffer. */
    class GLGpuBuffer : public GpuBuffer
    {
    public:
        virtual ~GLGpuBuffer();

        /**
         * Returns internal OpenGL buffer ID. If binding the buffer to the pipeline, bind the texture using
         * getGLTextureId() instead.
         */
        GLuint GetGLBufferId() const { return static_cast<GLHardwareBuffer*>(_buffer)->GetGLBufferId(); }

        /**	Returns internal OpenGL texture ID. */
        GLuint GetGLTextureId() const { return _textureID; }

        /** Returns the internal OpenGL format used by the elements of the buffer. */
        GLuint GetGLFormat() const { return _format; }

    protected:
        friend class GLHardwareBufferManager;

        GLGpuBuffer(const GPU_BUFFER_DESC& desc, GpuDeviceFlags deviceMask);
        GLGpuBuffer(const GPU_BUFFER_DESC& desc, SPtr<HardwareBuffer> underlyingBuffer);

        /** @copydoc GpuBuffer::Initialize */
        void Initialize() override;

        GLuint _textureID = 0;
        GLenum _format = 0;
    };
}
