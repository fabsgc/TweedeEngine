#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeRendererMaterial.h"
#include "Renderer/TeParamBlocks.h"
#include "Math/TeVector3.h"
#include "Math/TeMatrix4.h"

#define MAX_BULLET_DEBUG_INSTANCED_BLOCK 256

namespace te
{
    struct PerBulletDebugInstanceData
    {
        Vector4 FromColor;
        Vector4 ToColor;
        Vector3 From;
        float   Padding1;
        Vector3 To;
        float   Padding2;
    };

    TE_PARAM_BLOCK_BEGIN(PerBulletDebugFrameParamDef)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatViewProj)
        TE_PARAM_BLOCK_ENTRY(Vector3, gViewOrigin)
    TE_PARAM_BLOCK_END

    TE_PARAM_BLOCK_BEGIN(PerBulletDebugInstanceParamDef)
        TE_PARAM_BLOCK_ENTRY_ARRAY(PerBulletDebugInstanceData, gInstances, MAX_BULLET_DEBUG_INSTANCED_BLOCK)
    TE_PARAM_BLOCK_END

    /** Shader that performs Bullet debug rendering. */
    class BulletDebugMat : public RendererMaterial<BulletDebugMat>
    {
        RMAT_DEF(BuiltinShader::BulletDebug);

    public:
        using InstanceIter = Vector<PerBulletDebugInstanceData*>::iterator;

    public:
        BulletDebugMat();

        /** Set gpu params for camera */
        void BindCamera(const SPtr<Camera>& camera);

        /** Set gpu params for debug elements */
        void BindDebugElements(const InstanceIter& begin, const InstanceIter& end);

    private:
        PerBulletDebugFrameParamDef _perFrameParamDef;
        PerBulletDebugInstanceParamDef _perInstanceParamDef;

        SPtr<GpuParamBlockBuffer> _perFrameParamBuffer;
        SPtr<GpuParamBlockBuffer> _perInstanceParamBuffer;
    };
}
