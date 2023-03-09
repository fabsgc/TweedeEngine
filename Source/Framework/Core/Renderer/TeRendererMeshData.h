#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    enum class VertexLayout
    {
        Position = 0x01,
        Color = 0x02,
        Normal = 0x04,
        Tangent = 0x08,
        BiTangent = 0x10,
        BoneWeights = 0x20,
        UV0 = 0x40,
        UV1 = 0x80,
        PC = Position | Color,
        PU = Position | UV0,
        PCU = Position | Color | UV0,
        PCN = Position | Color | Normal,
        PCNU = Position | Color | Normal | UV0,
        PCNT = Position | Color | Normal | Tangent | BiTangent,
        PCNTU = Position | Color | Normal | Tangent | BiTangent | UV0,
        PN = Position | Normal,
        PNU = Position | Normal | UV0,
        PNT = Position | Normal | Tangent | BiTangent,
        PNTU = Position | Normal | Tangent | BiTangent | UV0,
    };

    class TE_CORE_EXPORT RendererMeshData : public NonCopyable
    {
    public:
        /**
         * Reads the vertex positions into the provided output buffer. Data will be copied and potentially uncompressed to
         * fit the output format as needed.
         *
         * @param[in]	buffer	Pre-allocated buffer to output the position data to.
         * @param[in]	size	Size of the pre-allocated buffer. Must be big enough to fit all contents (numVertices *
         *						sizeof(Vector3)).
         */
        void GetPositions(Vector3* buffer, UINT32 size);

        /**
         * Writes the vertex positions from the provided output buffer. Data will be copied and potentially compressed to
         * fit the internal mesh data format as needed.
         *
         * @param[in]	buffer	Pre-allocated buffer to read the position data from.
         * @param[in]	size	Size of the input buffer. Must be (numVertices * sizeof(Vector3)).
         */
        void SetPositions(Vector3* buffer, UINT32 size);

        /**
         * Reads the vertex normals into the provided output buffer. Data will be copied and potentially uncompressed to
         * fit the output format as needed.
         *
         * @param[in]	buffer	Pre-allocated buffer to output the normal data to.
         * @param[in]	size	Size of the pre-allocated buffer. Must be big enough to fit all contents (numVertices *
         *						sizeof(Vector3)).
         */
        void GetNormals(Vector3* buffer, UINT32 size);

        /**
         * Writes the vertex normals from the provided output buffer. Data will be copied and potentially compressed to
         * fit the internal mesh data format as needed.
         *
         * @param[in]	buffer	Pre-allocated buffer to read the normal data from.
         * @param[in]	size	Size of the input buffer. Must be (numVertices * sizeof(Vector3)).
         */
        void SetNormals(Vector3* buffer, UINT32 size);

        /**
         * Reads the vertex tangents into the provided output buffer. Data will be copied and potentially uncompressed to
         * fit the output format as needed.
         *
         * @param[in]	buffer	Pre-allocated buffer to output the tangent data to.
         * @param[in]	size	Size of the pre-allocated buffer. Must be big enough to fit all contents (numVertices *
         *						sizeof(Vector4)).
         */
        void GetTangents(Vector4* buffer, UINT32 size);

        /**
         * Writes the vertex tangents from the provided output buffer. Data will be copied and potentially compressed to
         * fit the internal mesh data format as needed.
         *
         * @param[in]	buffer	Pre-allocated buffer to read the tangent data from.
         * @param[in]	size	Size of the input buffer. Must be (numVertices * sizeof(Vector4)).
         */
        void SetTangents(Vector4* buffer, UINT32 size);

        /**
         * Reads the vertex bitangents into the provided output buffer. Data will be copied and potentially uncompressed to
         * fit the output format as needed.
         *
         * @param[in]	buffer	Pre-allocated buffer to output the tangent data to.
         * @param[in]	size	Size of the pre-allocated buffer. Must be big enough to fit all contents (numVertices *
         *						sizeof(Vector4)).
         */
        void GetBiTangents(Vector4* buffer, UINT32 size);

        /**
         * Writes the vertex bitangents from the provided output buffer. Data will be copied and potentially compressed to
         * fit the internal mesh data format as needed.
         *
         * @param[in]	buffer	Pre-allocated buffer to read the tangent data from.
         * @param[in]	size	Size of the input buffer. Must be (numVertices * sizeof(Vector4)).
         */
        void SetBiTangents(Vector4* buffer, UINT32 size);

        /**
         * Reads the vertex colors into the provided output buffer. Data will be copied and potentially uncompressed to
         * fit the output format as needed.
         *
         * @param[in]	buffer	Pre-allocated buffer to output the color data to.
         * @param[in]	size	Size of the pre-allocated buffer. Must be big enough to fit all contents (numVertices *
         *						sizeof(Color)).
         */
        void GetColors(Color* buffer, UINT32 size);

        /**
         * Reads the vertex colors into the provided output buffer. Data will be copied and potentially uncompressed to
         * fit the output format as needed.
         *
         * @param[in]	buffer	Pre-allocated buffer to output the color data to.
         * @param[in]	size	Size of the pre-allocated buffer. Must be big enough to fit all contents (numVertices *
         *						sizeof(Color)).
         */
        void GetColors(Vector4* buffer, UINT32 size);

        /**
         * Writes the vertex colors from the provided output buffer. Data will be copied and potentially compressed to
         * fit the internal mesh data format as needed.
         *
         * @param[in]	buffer	Pre-allocated buffer to read the color data from.
         * @param[in]	size	Size of the input buffer. Must be (numVertices * sizeof(Color)).
         */
        void SetColors(Color* buffer, UINT32 size);

        /**
         * Writes the vertex colors from the provided output buffer. Data will be copied and potentially compressed to
         * fit the internal mesh data format as needed.
         *
         * @param[in]	buffer	Pre-allocated buffer to read the color data from.
         * @param[in]	size	Size of the input buffer. Must be (numVertices * sizeof(Color)).
         */
        void SetColors(Vector4* buffer, UINT32 size);

        /**
         * Reads the first UV channel coordinates into the provided output buffer. Data will be copied and potentially
         * uncompressed to fit the output format as needed.
         *
         * @param[in]	buffer	Pre-allocated buffer to output the coordinate data to.
         * @param[in]	size	Size of the pre-allocated buffer. Must be big enough to fit all contents (numVertices *
         *						sizeof(Vector2)).
         */
        void GetUV0(Vector2* buffer, UINT32 size);

        /**
         * Writes the first UV channel coordinates from the provided output buffer. Data will be copied and potentially
         * compressed to fit the internal mesh data format as needed.
         *
         * @param[in]	buffer	Pre-allocated buffer to read the coordinate data from.
         * @param[in]	size	Size of the input buffer. Must be (numVertices * sizeof(Vector2)).
         */
        void SetUV0(Vector2* buffer, UINT32 size);

        /**
         * Reads the second UV channel coordinates into the provided output buffer. Data will be copied and potentially
         * uncompressed to fit the output format as needed.
         *
         * @param[in]	buffer	Pre-allocated buffer to output the coordinate data to.
         * @param[in]	size	Size of the pre-allocated buffer. Must be big enough to fit all contents (numVertices *
         *						sizeof(Vector2)).
         */
        void GetUV1(Vector2* buffer, UINT32 size);

        /**
         * Writes the second UV channel coordinates from the provided output buffer. Data will be copied and potentially
         * compressed to fit the internal mesh data format as needed.
         *
         * @param[in]	buffer	Pre-allocated buffer to read the coordinate data from.
         * @param[in]	size	Size of the input buffer. Must be (numVertices * sizeof(Vector2)).
         */
        void SetUV1(Vector2* buffer, UINT32 size);

        /**
         * Reads the bone weights and indices into the provided output buffer. Data will be copied and potentially
         * uncompressed to fit the output format as needed.
         *
         * @param[in]	buffer	Pre-allocated buffer to output the bone weight data to.
         * @param[in]	size	Size of the pre-allocated buffer. Must be big enough to fit all contents (numVertices *
         *						sizeof(BoneWeight)).
         */
        void GetBoneWeights(BoneWeight* buffer, UINT32 size);

        /**
         * Writes the bone weights and indices from the provided output buffer. Data will be copied and potentially
         * compressed to fit the internal mesh data format as needed.
         *
         * @param[in]	buffer	Pre-allocated buffer to read the bone weight data from.
         * @param[in]	size	Size of the input buffer. Must be (numVertices * sizeof(BoneWeight)).
         */
        void SetBoneWeights(BoneWeight* buffer, UINT32 size);

        /**
         * Reads the indices into the provided output buffer. Data will be copied and potentially uncompressed to fit the
         * output format as needed.
         *
         * @param[in]	buffer	Pre-allocated buffer to output the index data to.
         * @param[in]	size	Size of the pre-allocated buffer. Must be big enough to fit all contents (numVertices *
         *						sizeof(INT32)).
         */
        void GetIndices(UINT32* buffer, UINT32 size);

        /**
         * Writes the indices from the provided output buffer. Data will be copied and potentially compressed to fit the
         * internal mesh data format as needed.
         *
         * @param[in]	buffer	Pre-allocated buffer to read the index data from.
         * @param[in]	size	Size of the input buffer. Must be (numVertices * sizeof(INT32)).
         */
        void SetIndices(UINT32* buffer, UINT32 size);

        /**	Returns the underlying MeshData structure. */
        SPtr<MeshData> GetData() const { return _meshData; }

        /**	Creates a new empty mesh data structure. */
        static SPtr<RendererMeshData> Create(UINT32 numVertices, UINT32 numIndices, VertexLayout layout, IndexType indexType = IT_32BIT);

        /**	Creates a new mesh data structure using an existing mesh data buffer. */
        static SPtr<RendererMeshData> Create(const SPtr<MeshData>& meshData);

        /**	Creates a vertex descriptor from a vertex layout enum. */
        static SPtr<VertexDataDesc> VertexLayoutVertexDesc(VertexLayout type);

    private:
        friend class Renderer;

        RendererMeshData(UINT32 numVertices, UINT32 numIndices, VertexLayout layout, IndexType indexType = IT_32BIT);
        RendererMeshData(const SPtr<MeshData>& meshData);

        SPtr<MeshData> _meshData;
    };
}
