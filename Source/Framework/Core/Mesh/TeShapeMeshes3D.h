#pragma once

#include "TeCorePrerequisites.h"
#include "Image/TeColor.h"
#include "Math/TeAABox.h"
#include "Math/TeRect3.h"

namespace te
{
    /**	Helper class for easily creating common 3D shapes. */
    class TE_CORE_EXPORT ShapeMeshes3D
    {
    public:
        /**
         * Fills the provided buffers with position and index data representing a solid axis aligned box. Use
         * getNumElementsAABox() to determine the required sizes of the output buffers.
         *
         * @param[in]	box				Box to create geometry for.
         * @param[out]	outVertices		Pre-allocated output buffer that will store the vertex position data.
         * @param[out]	outNormals		Pre-allocated output buffer that will store the vertex normal data.
         * @param[out]	outUV			Pre-allocated output buffer that will store the vertex UV data. Set to null if not
         *								required.
         * @param[in]	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
         * @param[in]	vertexStride	Size of a single vertex, in bytes. (Same for both position and normal buffer)
         * @param[out]	outIndices		Pre-allocated output buffer that will store the index data. Indices are 32bit.
         * @param[in]	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
         */
        static void SolidAABox(const AABox& box, UINT8* outVertices, UINT8* outNormals, UINT8* outUV, UINT32 vertexOffset,
            UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset);

        /**	Calculates number of vertices and indices required for geometry of a solid axis aligned box. */
        static void GetNumElementsAABox(UINT32& numVertices, UINT32& numIndices);
    };
}
