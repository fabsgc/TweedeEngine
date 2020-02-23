#pragma once

#include "TeCorePrerequisites.h"
#include "Math/TeVector3.h"

namespace te
{
    struct VertexFaces
    {
        UINT32* Faces;
        UINT32 NumFaces = 0;
    };

    struct VertexConnectivity
    {
        VertexConnectivity(UINT8* indices, UINT32 numVertices, UINT32 NumFaces, UINT32 indexSize)
            :vertexFaces(nullptr), _maxFacesPerVertex(0), _numVertices(numVertices), _faces(nullptr)
        {
            vertexFaces = te_newN<VertexFaces>(numVertices);

            resizeFaceArray(10);

            for (UINT32 i = 0; i < NumFaces; i++)
            {
                for (UINT32 j = 0; j < 3; j++)
                {
                    UINT32 idx = i * 3 + j;
                    UINT32 vertexIdx = 0;
                    memcpy(&vertexIdx, indices + idx * indexSize, indexSize);

                    assert(vertexIdx < _numVertices);
                    VertexFaces& faces = vertexFaces[vertexIdx];
                    if (faces.NumFaces >= _maxFacesPerVertex)
                        resizeFaceArray(_maxFacesPerVertex * 2);

                    faces.Faces[faces.NumFaces] = i;
                    faces.NumFaces++;
                }
            }
        }

        ~VertexConnectivity()
        {
            if (vertexFaces != nullptr)
                te_deleteN(vertexFaces, _numVertices);

            if (_faces != nullptr)
                te_free(_faces);
        }

        VertexFaces* vertexFaces;

    private:
        void resizeFaceArray(UINT32 numFaces)
        {
            UINT32* newFaces = (UINT32*)te_allocate(numFaces * _numVertices * sizeof(UINT32));

            if (_faces != nullptr)
            {
                for (UINT32 i = 0; i < _numVertices; i++)
                    memcpy(newFaces + (i * numFaces), _faces + (i * _maxFacesPerVertex), _maxFacesPerVertex * sizeof(UINT32));

                te_free(_faces);
            }

            for (UINT32 i = 0; i < _numVertices; i++)
                vertexFaces[i].Faces = newFaces + (i * numFaces);

            _faces = newFaces;
            _maxFacesPerVertex = numFaces;
        }

        UINT32 _maxFacesPerVertex;
        UINT32 _numVertices;
        UINT32* _faces;
    };

    /** Performs various operations on mesh geometry. */
    class TE_CORE_EXPORT MeshUtility
    {
    public:
        /**
         * Calculates per-vertex normals based on the provided vertices and indices.
         *
         * @param[in]	vertices	Set of vertices containing vertex positions.
         * @param[in]	indices		Set of indices containing indexes into vertex array for each triangle.
         * @param[in]	numVertices	Number of vertices in the @p vertices array.
         * @param[in]	numIndices	Number of indices in the @p indices array. Must be a multiple of three.
         * @param[out]	normals		Pre-allocated buffer that will contain the calculated normals. Must be the same size
         *							as the vertex array.
         * @param[in]	indexSize	Size of a single index in the indices array, in bytes.
         *
         * @note
         * Vertices should be split before calling this method if there are any discontinuities. (for example a vertex on a
         * corner of a cube should be split into three vertices used by three triangles in order for the normals to be
         * valid.)
         */
        static void CalculateNormals(Vector3* vertices, UINT8* indices, UINT32 numVertices,
            UINT32 numIndices, Vector3* normals, UINT32 indexSize = 4);

        /**
         * Calculates per-vertex tangents and bitangents based on the provided vertices, uv coordinates and indices.
         *
         * @param[in]	vertices		Set of vertices containing vertex positions.
         * @param[in]	normals			Set of normals to use when calculating tangents. Must the the same length as the
         *								number of vertices.
         * @param[in]	uv				Set of UV coordinates to use when calculating tangents. Must the the same length as
         *								the number of vertices.
         * @param[in]	indices			Set of indices containing indexes into vertex array for each triangle.
         * @param[in]	numVertices		Number of vertices in the @p vertices, @p normals and @p uv arrays.
         * @param[in]	numIndices		Number of indices in the @p indices array. Must be a multiple of three.
         * @param[out]	tangents		Pre-allocated buffer that will contain the calculated tangents. Must be the same
         *								size as the vertex array.
         * @param[out]	bitangents		Pre-allocated buffer that will contain the calculated bitangents. Must be the same
         *								size as the vertex array.
         * @param[in]	indexSize		Size of a single index in the indices array, in bytes.
         * @param[in]	vertexStride	Number of bytes to advance the @p vertices, @p normals and @p uv arrays with each
         *								vertex. If set to zero them each array is advanced according to its own size.
         *
         * @note
         * Vertices should be split before calling this method if there are any discontinuities. (for example a vertex on a
         * corner of a cube should be split into three vertices used by three triangles in order for the normals to be
         * valid.)
         */
        static void CalculateTangents(Vector3* vertices, Vector3* normals, Vector2* uv, UINT8* indices, UINT32 numVertices,
            UINT32 numIndices, Vector3* tangents, Vector3* bitangents, UINT32 indexSize = 4, UINT32 vertexStride = 0);

        /**
         * Calculates per-vertex tangent space (normal, tangent, bitangent) based on the provided vertices, uv coordinates
         * and indices.
         *
         * @param[in]	vertices	Set of vertices containing vertex positions.
         * @param[in]	uv			Set of UV coordinates to use when calculating tangents.
         * @param[in]	indices		Set of indices containing indexes into vertex array for each triangle.
         * @param[in]	numVertices	Number of vertices in the "vertices" array.
         * @param[in]	numIndices	Number of indices in the "indices" array. Must be a multiple of three.
         * @param[out]	normals		Pre-allocated buffer that will contain the calculated normals. Must be the same size
         *							as the vertex array.
         * @param[out]	tangents	Pre-allocated buffer that will contain the calculated tangents. Must be the same size
         *							as the vertex array.
         * @param[out]	bitangents	Pre-allocated buffer that will contain the calculated bitangents. Must be the same size
         *							as the vertex array.
         * @param[in]	indexSize	Size of a single index in the indices array, in bytes.
         *
         * @note
         * Vertices should be split before calling this method if there are any discontinuities. (for example. a vertex on
         * a corner of a cube should be split into three vertices used by three triangles in order for the normals to be
         * valid.)
         */
        static void CalculateTangentSpace(Vector3* vertices, Vector2* uv, UINT8* indices, UINT32 numVertices,
            UINT32 numIndices, Vector3* normals, Vector3* tangents, Vector3* bitangents, UINT32 indexSize = 4);
    };
}
