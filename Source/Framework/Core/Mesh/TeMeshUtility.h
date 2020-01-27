#pragma once

#include "TeCorePrerequisites.h"
#include "Math/TeVector3.h"

namespace te
{
    union PackedNormal
    {
        struct
        {
            UINT8 x;
            UINT8 y;
            UINT8 z;
            UINT8 w;
        };

        UINT32 packed;
    };

    /** Performs various operations on mesh geometry. */
    class TE_CORE_EXPORT MeshUtility
    {
    public:
        /**
         * Encodes normals from 32-bit float format into 4D 8-bit packed format.
         *
         * @param[in]	source			Buffer containing data to encode. Must have @p count entries.
         * @param[out]	destination		Buffer to output the data to. Must have @p count entries, each 32-bits.
         * @param[in]	count			Number of entries in the @p source and @p destination arrays.
         * @param[in]	inStride		Distance between two entries in the @p source buffer, in bytes.
         * @param[in]	outStride		Distance between two entries in the @p destination buffer, in bytes.
         */
        static void PackNormals(Vector3* source, UINT8* destination, UINT32 count, UINT32 inStride, UINT32 outStride);

        /**
         * Encodes normals from 32-bit float format into 4D 8-bit packed format.
         *
         * @param[in]	source			Buffer containing data to encode. Must have @p count entries.
         * @param[out]	destination		Buffer to output the data to. Must have @p count entries, each 32-bits.
         * @param[in]	count			Number of entries in the @p source and @p destination arrays.
         * @param[in]	inStride		Distance between two entries in the @p source buffer, in bytes.
         * @param[in]	outStride		Distance between two entries in the @p destination buffer, in bytes.
         */
        static void PackNormals(Vector4* source, UINT8* destination, UINT32 count, UINT32 inStride, UINT32 outStride);

        /**
         * Decodes normals from 4D 8-bit packed format into a 32-bit float format.
         *
         * @param[in]	source			Buffer containing data to encode. Must have @p count entries, each 32-bits.
         * @param[out]	destination		Buffer to output the data to. Must have @p count entries.
         * @param[in]	count			Number of entries in the @p source and @p destination arrays.
         * @param[in]	stride			Distance between two entries in the @p source buffer, in bytes.
         */
        static void UnpackNormals(UINT8* source, Vector3* destination, UINT32 count, UINT32 stride);

        /**
         * Decodes normals from 4D 8-bit packed format into a 32-bit float format.
         *
         * @param[in]	source			Buffer containing data to encode. Must have @p count entries, each 32-bits.
         * @param[out]	destination		Buffer to output the data to. Must have @p count entries.
         * @param[in]	count			Number of entries in the @p source and @p destination arrays.
         * @param[in]	stride			Distance between two entries in the @p source buffer, in bytes.
         */
        static void UnpackNormals(UINT8* source, Vector4* destination, UINT32 count, UINT32 stride);

        /** Decodes a normal from 4D 8-bit packed format into a 32-bit float format. */
        static Vector3 UnpackNormal(const UINT8* source)
        {
            const PackedNormal& packed = *(PackedNormal*)source;
            Vector3 output;

            const float inv = (1.0f / 255.0f) * 2.0f;
            output.x = (packed.x * inv - 1.0f);
            output.y = (packed.y * inv - 1.0f);
            output.z = (packed.z * inv - 1.0f);

            return output;
        }
    };
}
