#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "RenderAPI/TeVertexBuffer.h"
#include "TeGLHardwareBuffer.h"
#include "TeGLVertexArrayObjectManager.h"

namespace te
{
    /** OpenGL implementation of a vertex buffer. */
    class GLVertexBuffer : public VertexBuffer
    {
    public:
        GLVertexBuffer(const VERTEX_BUFFER_DESC& desc, GpuDeviceFlags deviceMask);
        ~GLVertexBuffer();

        /**	Returns internal OpenGL buffer ID. */
        GLuint GetGLBufferId() const { return static_cast<GLHardwareBuffer*>(_buffer)->GetGLBufferId(); }

        /**	Registers a new VertexArrayObject that uses this vertex buffer. */
        void RegisterVAO(const GLVertexArrayObject& vao);

        /**	Unregisters a VAO from this vertex buffer. Does not destroy it. */
        void UnregisterVAO(const GLVertexArrayObject& vao);

    protected:
        /** @copydoc VertexBuffer::Initialize */
        void Initialize() override;

    private:
        Vector<GLVertexArrayObject> _VAObjects;
    };
}
