#include "Mesh/TeMeshUtility.h"
#include "Math/TeVector4.h"
#include "Math/TeVector3.h"
#include "Math/TeVector2.h"

namespace te
{
    void MeshUtility::PackNormals(Vector3* source, UINT8* destination, UINT32 count, UINT32 inStride, UINT32 outStride)
    {
        UINT8* srcPtr = (UINT8*)source;
        UINT8* dstPtr = destination;
        for (UINT32 i = 0; i < count; i++)
        {
            Vector3 src = *(Vector3*)srcPtr;

            PackedNormal& packed = *(PackedNormal*)dstPtr;
            packed.x = Math::Clamp((int)(src.x * 127.5f + 127.5f), 0, 255);
            packed.y = Math::Clamp((int)(src.y * 127.5f + 127.5f), 0, 255);
            packed.z = Math::Clamp((int)(src.z * 127.5f + 127.5f), 0, 255);
            packed.w = 128;

            srcPtr += inStride;
            dstPtr += outStride;
        }
    }

    void MeshUtility::PackNormals(Vector4* source, UINT8* destination, UINT32 count, UINT32 inStride, UINT32 outStride)
    {
        UINT8* srcPtr = (UINT8*)source;
        UINT8* dstPtr = destination;
        for (UINT32 i = 0; i < count; i++)
        {
            Vector4 src = *(Vector4*)srcPtr;
            PackedNormal& packed = *(PackedNormal*)dstPtr;

            packed.x = Math::Clamp((int)(src.x * 127.5f + 127.5f), 0, 255);
            packed.y = Math::Clamp((int)(src.y * 127.5f + 127.5f), 0, 255);
            packed.z = Math::Clamp((int)(src.z * 127.5f + 127.5f), 0, 255);
            packed.w = Math::Clamp((int)(src.w * 127.5f + 127.5f), 0, 255);

            srcPtr += inStride;
            dstPtr += outStride;
        }
    }

    void MeshUtility::UnpackNormals(UINT8* source, Vector3* destination, UINT32 count, UINT32 stride)
    {
        UINT8* ptr = source;
        for (UINT32 i = 0; i < count; i++)
        {
            destination[i] = UnpackNormal(ptr);

            ptr += stride;
        }
    }

    void MeshUtility::UnpackNormals(UINT8* source, Vector4* destination, UINT32 count, UINT32 stride)
    {
        UINT8* ptr = source;
        for (UINT32 i = 0; i < count; i++)
        {
            PackedNormal& packed = *(PackedNormal*)ptr;

            const float inv = (1.0f / 255.0f) * 2.0f;
            destination[i].x = (packed.x * inv - 1.0f);
            destination[i].y = (packed.y * inv - 1.0f);
            destination[i].z = (packed.z * inv - 1.0f);
            destination[i].w = (packed.w * inv - 1.0f);

            ptr += stride;
        }
    }
}
