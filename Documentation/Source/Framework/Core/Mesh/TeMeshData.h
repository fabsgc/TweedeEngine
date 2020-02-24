#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeGpuResourceData.h"
#include "RenderAPI/TeVertexBuffer.h"
#include "RenderAPI/TeIndexBuffer.h"
#include "RenderAPI/TeVertexDeclaration.h"
#include "RenderAPI/TeSubMesh.h"
#include "Math/TeBounds.h"

namespace te
{
    /** Contains mesh vertex and index data used for initializing, updating and reading mesh data from Mesh. */
    class TE_CORE_EXPORT MeshData : public GpuResourceData
    {
    public:
        /**
         * Constructs a new object that can hold number of vertices described by the provided vertex data description. As
         * well as a number of indices of the provided type.
         */
        MeshData(UINT32 numVertices, UINT32 numIndexes, const SPtr<VertexDataDesc>& vertexData, IndexType indexType = IT_32BIT);
        ~MeshData();

        /**
         * Copies data from @p data parameter into the internal buffer for the specified semantic.
         *
         * @param[in]	semantic   		Semantic that allows the engine to connect the data to a shader input slot.
         * @param[in]	data			Vertex data, containing at least @p size bytes.
         * @param[in]	size			The size of the data. Must be the size of the vertex element type * number of
         *								vertices.
         * @param[in]	semanticIdx 	(optional) If there are multiple semantics with the same name, use different index
         *								to differentiate between them.
         * @param[in]	streamIdx   	(optional) Zero-based index of the stream. Each stream will internally be
         *								represented as a single vertex buffer.
         */
        void SetVertexData(VertexElementSemantic semantic, void* data, UINT32 size, UINT32 semanticIdx = 0, UINT32 streamIdx = 0);

        /**
         * Copies data from the internal buffer to the pre-allocated buffer for the specified semantic.
         *
         * @param[in]	semantic   		Semantic that allows the engine to connect the data to a shader input slot.
         * @param[in]	data			Buffer that will receive vertex data, of at least @p size bytes.
         * @param[in]	size			The size of the data. Must be the size of the vertex element type * number of
         *								vertices.
         * @param[in]	semanticIdx 	(optional) If there are multiple semantics with the same name, use different index
         *								to differentiate between them.
         * @param[in]	streamIdx   	(optional) Zero-based index of the stream. Each stream will internally be
         *								represented as a single vertex buffer.
         */
        void GetVertexData(VertexElementSemantic semantic, void* data, UINT32 size, UINT32 semanticIdx = 0, UINT32 streamIdx = 0);

        /** Returns the total number of vertices this object can hold. */
        UINT32 GetNumVertices() const { return _numVertices; }

        /** Returns the total number of indices this object can hold. */
        UINT32 GetNumIndices() const;

        /**	Returns a 16-bit pointer to the start of the internal index buffer. */
        UINT16* GetIndices16() const;

        /**	Returns a 32-bit pointer to the start of the internal index buffer. */
        UINT32* GetIndices32() const;

        /**	Returns the size of an index element in bytes. */
        UINT32 GetIndexElementSize() const;

        /**	Returns the type of an index element. */
        IndexType GetIndexType() const { return _indexType; }

        /**
         * Returns the pointer to the first element of the specified type. If you want to iterate over all elements you
         * need to call getVertexStride() to get the number	of bytes you need to advance between each element.
         *
         * @param[in]	semantic   		Semantic that allows the engine to connect the data to a shader input slot.
         * @param[in]	semanticIdx 	(optional) If there are multiple semantics with the same name, use different index
         *								to differentiate between them.
         * @param[in]	streamIdx   	(optional) Zero-based index of the stream. Each stream will internally be
         *								represented as a single vertex buffer.
         * @return						null if it fails, else the element data.
         */
        UINT8* GetElementData(VertexElementSemantic semantic, UINT32 semanticIdx = 0, UINT32 streamIdx = 0) const;

        /**
         * Returns an offset into the internal buffer where this element with the provided semantic starts. Offset is
         * provided in number of bytes.
         *
         * @param[in]	semantic   		Semantic that allows the engine to connect the data to a shader input slot.
         * @param[in]	semanticIdx 	(optional) If there are multiple semantics with the same name, use different index
         *								to differentiate between them.
         * @param[in]	streamIdx   	(optional) Zero-based index of the stream. Each stream will internally be
         *								represented as a single vertex buffer.
         */
        UINT32 GetElementOffset(VertexElementSemantic semantic, UINT32 semanticIdx = 0, UINT32 streamIdx = 0) const;

        /**	Returns a pointer to the start of the index buffer. */
        UINT8* GetIndexData() const { return GetData(); }

        /**	Returns a pointer to the start of the specified vertex stream. */
        UINT8* GetStreamData(UINT32 streamIdx) const;

        /**	Returns the size of the specified stream in bytes. */
        UINT32 GetStreamSize(UINT32 streamIdx) const;

        /**	Returns the size of all the streams in bytes. */
        UINT32 GetStreamSize() const;

        /** Returns an object that describes data contained in a single vertex. */
        const SPtr<VertexDataDesc>& GetVertexDesc() const { return _vertexData; }

        /**	Return the size (in bytes) of the entire buffer. */
        UINT32 GetSize() const { return GetInternalBufferSize(); }

        /**	Calculates the bounds of all vertices stored in the internal buffer. */
        Bounds CalculateBounds() const;

        /**
         * Combines a number of submeshes and their mesh data into one large mesh data buffer.
         *
         * @param[in]	elements		Data containing vertices and indices referenced by the submeshes. Number of elements
         *								must be the same as number of submeshes.
         * @param[in]	allSubMeshes	Submeshes representing vertex and index range to take from mesh data and combine.
         *								Number of submeshes must match the number of provided MeshData elements.
         * @param[out]	subMeshes		Outputs all combined sub-meshes with their new index and vertex offsets referencing
         *								the newly created MeshData.
         * @return						Combined mesh data containing all vertices and indexes references by the provided
         *								sub-meshes.
         */
        static SPtr<MeshData> Combine(const Vector<SPtr<MeshData>>& elements, const Vector<Vector<SubMesh>>& allSubMeshes,
            Vector<SubMesh>& subMeshes);

        /**
         * Constructs a new object that can hold number of vertices described by the provided vertex data description. As
         * well as a number of indices of the provided type.
         */
        static SPtr<MeshData> Create(UINT32 numVertices, UINT32 numIndexes, const SPtr<VertexDataDesc>& vertexData,
            IndexType indexType = IT_32BIT)
        {
            return te_shared_ptr_new<MeshData>(numVertices, numIndexes, vertexData, indexType);
        }

    protected:
        /**	Returns the size of the internal buffer in bytes. */
        UINT32 GetInternalBufferSize() const override;

    private:
        /**	Returns an offset in bytes to the start of the index buffer from the start of the internal buffer. */
        UINT32 GetIndexBufferOffset() const;

        /**	Returns an offset in bytes to the start of the stream from the start of the internal buffer. */
        UINT32 GetStreamOffset(UINT32 streamIdx = 0) const;

        /**	Returns the size of the index buffer in bytes. */
        UINT32 GetIndexBufferSize() const;

    private:
        friend class Mesh;

        UINT32 _dscBuilding;

        UINT32 _numVertices;
        UINT32 _numIndices;
        IndexType _indexType;

        SPtr<VertexDataDesc> _vertexData;
    };
}
