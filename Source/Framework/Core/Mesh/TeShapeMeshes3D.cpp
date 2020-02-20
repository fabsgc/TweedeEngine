#include "Mesh/TeShapeMeshes3D.h"
#include "Math/TeRect2.h"
#include "Mesh/TeMesh.h"
#include "Math/TeVector2.h"
#include "Math/TeQuaternion.h"
#include "RenderAPI/TeVertexDataDesc.h"
#include "Mesh/TeMeshUtility.h"

namespace te
{
    inline UINT8* WriteVector3(UINT8* buffer, UINT32 stride, const Vector3& value)
    {
        *(Vector3*)buffer = value;
        return buffer + stride;
    }

    inline UINT8* WriteVector2(UINT8* buffer, UINT32 stride, const Vector2& value)
    {
        *(Vector2*)buffer = value;
        return buffer + stride;
    }

    void ShapeMeshes3D::SolidAABox(const AABox& box, UINT8* outVertices, UINT8* outNormals, UINT8* outUVs,
        UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset)
    {
        outVertices += (vertexOffset * vertexStride);

        // Front face
        outVertices = WriteVector3(outVertices, vertexStride, box.GetCorner(AABox::NEAR_LEFT_BOTTOM));
        outVertices = WriteVector3(outVertices, vertexStride, box.GetCorner(AABox::NEAR_RIGHT_BOTTOM));
        outVertices = WriteVector3(outVertices, vertexStride, box.GetCorner(AABox::NEAR_RIGHT_TOP));
        outVertices = WriteVector3(outVertices, vertexStride, box.GetCorner(AABox::NEAR_LEFT_TOP));

        // Back face
        outVertices = WriteVector3(outVertices, vertexStride, box.GetCorner(AABox::FAR_RIGHT_BOTTOM));
        outVertices = WriteVector3(outVertices, vertexStride, box.GetCorner(AABox::FAR_LEFT_BOTTOM));
        outVertices = WriteVector3(outVertices, vertexStride, box.GetCorner(AABox::FAR_LEFT_TOP));
        outVertices = WriteVector3(outVertices, vertexStride, box.GetCorner(AABox::FAR_RIGHT_TOP));

        // Left face
        outVertices = WriteVector3(outVertices, vertexStride, box.GetCorner(AABox::FAR_LEFT_BOTTOM));
        outVertices = WriteVector3(outVertices, vertexStride, box.GetCorner(AABox::NEAR_LEFT_BOTTOM));
        outVertices = WriteVector3(outVertices, vertexStride, box.GetCorner(AABox::NEAR_LEFT_TOP));
        outVertices = WriteVector3(outVertices, vertexStride, box.GetCorner(AABox::FAR_LEFT_TOP));

        // Right face
        outVertices = WriteVector3(outVertices, vertexStride, box.GetCorner(AABox::NEAR_RIGHT_BOTTOM));
        outVertices = WriteVector3(outVertices, vertexStride, box.GetCorner(AABox::FAR_RIGHT_BOTTOM));
        outVertices = WriteVector3(outVertices, vertexStride, box.GetCorner(AABox::FAR_RIGHT_TOP));
        outVertices = WriteVector3(outVertices, vertexStride, box.GetCorner(AABox::NEAR_RIGHT_TOP));

        // Top face
        outVertices = WriteVector3(outVertices, vertexStride, box.GetCorner(AABox::FAR_LEFT_TOP));
        outVertices = WriteVector3(outVertices, vertexStride, box.GetCorner(AABox::NEAR_LEFT_TOP));
        outVertices = WriteVector3(outVertices, vertexStride, box.GetCorner(AABox::NEAR_RIGHT_TOP));
        outVertices = WriteVector3(outVertices, vertexStride, box.GetCorner(AABox::FAR_RIGHT_TOP));

        // Bottom face
        outVertices = WriteVector3(outVertices, vertexStride, box.GetCorner(AABox::FAR_LEFT_BOTTOM));
        outVertices = WriteVector3(outVertices, vertexStride, box.GetCorner(AABox::FAR_RIGHT_BOTTOM));
        outVertices = WriteVector3(outVertices, vertexStride, box.GetCorner(AABox::NEAR_RIGHT_BOTTOM));
        outVertices = WriteVector3(outVertices, vertexStride, box.GetCorner(AABox::NEAR_LEFT_BOTTOM));

        // Normals
        static const Vector3 faceNormals[6] =
        {
            Vector3(0, 0, 1),
            Vector3(0, 0, -1),
            Vector3(-1, 0, 0),
            Vector3(1, 0, 0),
            Vector3(0, 1, 0),
            Vector3(0, -1, 0)
        };

        if (outNormals != nullptr)
        {
            outNormals += (vertexOffset * vertexStride);
            for (UINT32 face = 0; face < 6; face++)
            {
                outNormals = WriteVector3(outNormals, vertexStride, faceNormals[face]);
                outNormals = WriteVector3(outNormals, vertexStride, faceNormals[face]);
                outNormals = WriteVector3(outNormals, vertexStride, faceNormals[face]);
                outNormals = WriteVector3(outNormals, vertexStride, faceNormals[face]);
            }
        }

        // UV
        if (outUVs != nullptr)
        {
            outUVs += (vertexOffset * vertexStride);
            for (UINT32 face = 0; face < 6; face++)
            {
                outUVs = WriteVector2(outUVs, vertexStride, Vector2(0.0f, 1.0f));
                outUVs = WriteVector2(outUVs, vertexStride, Vector2(1.0f, 1.0f));
                outUVs = WriteVector2(outUVs, vertexStride, Vector2(1.0f, 0.0f));
                outUVs = WriteVector2(outUVs, vertexStride, Vector2(0.0f, 0.0f));
            }
        }

        // Indices
        UINT32* indices = outIndices + indexOffset;
        for (UINT32 face = 0; face < 6; face++)
        {
            UINT32 faceVertOffset = vertexOffset + face * 4;

            indices[face * 6 + 0] = faceVertOffset + 2;
            indices[face * 6 + 1] = faceVertOffset + 1;
            indices[face * 6 + 2] = faceVertOffset + 0;
            indices[face * 6 + 3] = faceVertOffset + 0;
            indices[face * 6 + 4] = faceVertOffset + 3;
            indices[face * 6 + 5] = faceVertOffset + 2;
        }
    }

    void ShapeMeshes3D::GetNumElementsAABox(UINT32& numVertices, UINT32& numIndices)
    {
        numVertices = 24;
        numIndices = 36;
    }
}
