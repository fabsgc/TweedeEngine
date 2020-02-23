#include "Mesh/TeMeshUtility.h"
#include "Math/TeVector4.h"
#include "Math/TeVector3.h"
#include "Math/TeVector2.h"

namespace te
{
    void MeshUtility::CalculateNormals(Vector3* vertices, UINT8* indices, UINT32 numVertices,
        UINT32 numIndices, Vector3* normals, UINT32 indexSize)
    {
        UINT32 numFaces = numIndices / 3;

        Vector3* faceNormals = te_newN<Vector3>(numFaces);
        for (UINT32 i = 0; i < numFaces; i++)
        {
            UINT32 triangle[3];
            memcpy(&triangle[0], indices + (i * 3 + 0) * indexSize, indexSize);
            memcpy(&triangle[1], indices + (i * 3 + 1) * indexSize, indexSize);
            memcpy(&triangle[2], indices + (i * 3 + 2) * indexSize, indexSize);

            Vector3 edgeA = vertices[triangle[1]] - vertices[triangle[0]];
            Vector3 edgeB = vertices[triangle[2]] - vertices[triangle[0]];
            faceNormals[i] = Vector3::Normalize(Vector3::Cross(edgeA, edgeB));

            // Note: Potentially don't normalize here in order to weigh the normals
            // by triangle size
        }

        VertexConnectivity connectivity(indices, numVertices, numFaces, indexSize);
        for (UINT32 i = 0; i < numVertices; i++)
        {
            VertexFaces& faces = connectivity.vertexFaces[i];

            normals[i] = Vector3::ZERO;
            for (UINT32 j = 0; j < faces.NumFaces; j++)
            {
                UINT32 faceIdx = faces.Faces[j];
                normals[i] += faceNormals[faceIdx];
            }

            normals[i].Normalize();
        }

        te_deleteN(faceNormals, numFaces);
    }

    void MeshUtility::CalculateTangents(Vector3* vertices, Vector3* normals, Vector2* uv, UINT8* indices, UINT32 numVertices,
        UINT32 numIndices, Vector3* tangents, Vector3* bitangents, UINT32 indexSize, UINT32 vertexStride)
    {
        UINT32 numFaces = numIndices / 3;
        UINT32 vec2Stride = vertexStride == 0 ? sizeof(Vector2) : vertexStride;
        UINT32 vec3Stride = vertexStride == 0 ? sizeof(Vector3) : vertexStride;

        UINT8* positionBytes = (UINT8*)vertices;
        UINT8* normalBytes = (UINT8*)normals;
        UINT8* uvBytes = (UINT8*)uv;

        Vector3* faceTangents = te_newN<Vector3>(numFaces);
        Vector3* faceBitangents = te_newN<Vector3>(numFaces);
        for (UINT32 i = 0; i < numFaces; i++)
        {
            UINT32 triangle[3];
            memcpy(&triangle[0], indices + (i * 3 + 0) * indexSize, indexSize);
            memcpy(&triangle[1], indices + (i * 3 + 1) * indexSize, indexSize);
            memcpy(&triangle[2], indices + (i * 3 + 2) * indexSize, indexSize);

            Vector3 p0 = *(Vector3*)&positionBytes[triangle[0] * vec3Stride];
            Vector3 p1 = *(Vector3*)&positionBytes[triangle[1] * vec3Stride];
            Vector3 p2 = *(Vector3*)&positionBytes[triangle[2] * vec3Stride];

            Vector2 uv0 = *(Vector2*)&uvBytes[triangle[0] * vec2Stride];
            Vector2 uv1 = *(Vector2*)&uvBytes[triangle[1] * vec2Stride];
            Vector2 uv2 = *(Vector2*)&uvBytes[triangle[2] * vec2Stride];

            Vector3 q0 = p1 - p0;
            Vector3 q1 = p2 - p0;

            Vector2 st1 = uv1 - uv0;
            Vector2 st2 = uv2 - uv0;

            float denom = st1.x * st2.y - st2.x * st1.y;
            if (fabs(denom) >= 0e-8f)
            {
                float r = 1.0f / denom;

                faceTangents[i] = (st2.y * q0 - st1.y * q1) * r;
                faceBitangents[i] = (st1.x * q1 - st2.x * q0) * r;

                faceTangents[i].Normalize();
                faceBitangents[i].Normalize();
            }

            // Note: Potentially don't normalize here in order to weight the normals by triangle size
        }

        VertexConnectivity connectivity(indices, numVertices, numFaces, indexSize);
        for (UINT32 i = 0; i < numVertices; i++)
        {
            VertexFaces& faces = connectivity.vertexFaces[i];

            tangents[i] = Vector3::ZERO;
            bitangents[i] = Vector3::ZERO;

            for (UINT32 j = 0; j < faces.NumFaces; j++)
            {
                UINT32 faceIdx = faces.Faces[j];
                tangents[i] += faceTangents[faceIdx];
                bitangents[i] += faceBitangents[faceIdx];
            }

            tangents[i].Normalize();
            bitangents[i].Normalize();

            Vector3 normal = *(Vector3*)&normalBytes[i * vec3Stride];

            // Orthonormalize
            float dot0 = normal.Dot(tangents[i]);
            tangents[i] -= dot0 * normal;
            tangents[i].Normalize();

            float dot1 = tangents[i].Dot(bitangents[i]);
            dot0 = normal.Dot(bitangents[i]);
            bitangents[i] -= dot0 * normal + dot1 * tangents[i];
            bitangents[i].Normalize();
        }

        te_deleteN(faceTangents, numFaces);
        te_deleteN(faceBitangents, numFaces);

        // TODO - Consider weighing tangents by triangle size and/or edge angles
    }

    void MeshUtility::CalculateTangentSpace(Vector3* vertices, Vector2* uv, UINT8* indices, UINT32 numVertices,
        UINT32 numIndices, Vector3* normals, Vector3* tangents, Vector3* bitangents, UINT32 indexSize)
    {
        CalculateNormals(vertices, indices, numVertices, numIndices, normals, indexSize);
        CalculateTangents(vertices, normals, uv, indices, numVertices, numIndices, tangents, bitangents, indexSize);
    }
}
