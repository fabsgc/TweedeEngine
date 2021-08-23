#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeParamBlocks.h"
#include "RenderAPI/TeVertexBuffer.h"
#include "RenderAPI/TeVertexDataDesc.h"
#include "RenderAPI/TeVertexDeclaration.h"

#define MAX_HUD_INSTANCED_BLOCK 256

namespace te
{
    namespace PickingUtils
    {
        enum class RenderType
        {
            Draw = 0x0,
            Selection = 0x1,
            Picking = 0x2,
        };

        enum class HudType
        {
            Camera = 0x0,
            RadialLight = 0x1,
            DirectionalLight = 0x2,
            SpotLight = 0x3,
            AudioListener = 0x4,
            AudioSource = 0x5
        };

        struct PerHudInstanceData
        {
            Matrix4 MatWorldNoScale;
            Vector4 Color;
            float   Type;
            Vector3 Padding;
        };

        struct VertexBufferLayout
        {
            Vector3 Position;
        };

        struct HudInstanceBuffer
        {
            SPtr<VertexBuffer> PointVB;
            SPtr<VertexDataDesc> PointVDesc;
            SPtr<VertexDeclaration> PointVDecl;
            VertexBufferLayout* PointData = nullptr;
        };

        TE_CORE_EXPORT void CreateHudInstanceBuffer(HudInstanceBuffer& buffer);

        TE_CORE_EXPORT void FillPerInstanceHud(Vector<PerHudInstanceData>& instancedElements, const HCamera& camera, const HComponent& component, RenderType renderType);

        TE_PARAM_BLOCK_BEGIN(PerPickSelectFrameParamDef)
            TE_PARAM_BLOCK_ENTRY(Matrix4, gMatViewProj)
            TE_PARAM_BLOCK_ENTRY(Vector3, gViewOrigin)
            TE_PARAM_BLOCK_ENTRY(UINT32, gRenderType)
        TE_PARAM_BLOCK_END

        TE_PARAM_BLOCK_BEGIN(PerPickSelectObjectParamDef)
            TE_PARAM_BLOCK_ENTRY(Matrix4, gMatWorld)
            TE_PARAM_BLOCK_ENTRY(Vector4, gColor)
            TE_PARAM_BLOCK_ENTRY(UINT32, gHasAnimation)
        TE_PARAM_BLOCK_END

        TE_PARAM_BLOCK_BEGIN(PerHudInstanceParamDef)
            TE_PARAM_BLOCK_ENTRY_ARRAY(PerHudInstanceData, gInstances, MAX_HUD_INSTANCED_BLOCK)
        TE_PARAM_BLOCK_END
    }
}
