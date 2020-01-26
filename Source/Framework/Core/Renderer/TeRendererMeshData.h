#pragma once

#include "TeCorePrerequisites.h"
#include "Mesh/TeMeshData.h"

namespace te
{
    enum class VertexLayout
    {
        Position = 0x01,
        Color = 0x02,
        Normal = 0x04,
        Tangent = 0x08,
        BoneWeights = 0x10,
        UV0 = 0x20,
        UV1 = 0x40,
        PC = Position | Color,
        PU = Position | UV0,
        PCU = Position | Color | UV0,
        PCN = Position | Color | Normal,
        PCNU = Position | Color | Normal | UV0,
        PCNT = Position | Color | Normal | Tangent,
        PCNTU = Position | Color | Normal | Tangent | UV0,
        PN = Position | Normal,
        PNU = Position | Normal | UV0,
        PNT = Position | Normal | Tangent,
        PNTU = Position | Normal | Tangent | UV0,
    };

    class RendererMeshData
    {
    public:
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
