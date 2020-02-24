#pragma once

#include "TeCorePrerequisites.h"
#include "RenderAPI/TeVertexDeclaration.h"
#include "RenderAPI/TeVertexBuffer.h"

namespace te
{
    /**
     * Container class consisting of a set of vertex buffers and their declaration.
     *
     * @note    Used just for more easily passing around vertex information.
     */
    class TE_CORE_EXPORT VertexData
    {
    public:
        VertexData() = default;
        ~VertexData() = default;

        /** Assigns a new vertex buffer to the specified index. */
        void SetBuffer(UINT32 index, SPtr<VertexBuffer> buffer);

        /** Retrieves a vertex buffer from the specified index. */
        SPtr<VertexBuffer> GetBuffer(UINT32 index) const;

        /** Returns a list of all bound vertex buffers. */
        const UnorderedMap<UINT32, SPtr<VertexBuffer>>& GetBuffers() const { return _vertexBuffers; }

        /** Checks if there is a buffer at the specified index. */
        bool IsBufferBound(UINT32 index) const;

        /** Gets total number of bound buffers. */
        UINT32 GetBufferCount() const { return (UINT32)_vertexBuffers.size(); }

        /** Returns the maximum index of all bound buffers. */
        UINT32 GetMaxBufferIndex() const { return _maxBufferIdx; }

        /** Declaration used for the contained vertex buffers. */
        SPtr<VertexDeclaration> vertexDeclaration;

        /** Number of vertices to use. */
        UINT32 vertexCount = 0;

    private:
        void RecalculateMaxIndex();

        UnorderedMap<UINT32, SPtr<VertexBuffer>> _vertexBuffers;
        UINT32 _maxBufferIdx = 0;
    };
}
