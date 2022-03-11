#include "TeRendererRenderable.h"
#include "Renderer/TeRendererUtility.h"
#include "Utility/TeBitwise.h"
#include "Mesh/TeMesh.h"

namespace te
{ 
    PerInstanceParamDef gPerInstanceParamDef;
    PerMaterialParamDef gPerMaterialParamDef;
    PerObjectParamDef gPerObjectParamDef;

    void PerObjectBuffer::UpdatePerObject(SPtr<GpuParamBlockBuffer>& buffer, const Matrix4& tfrm,
        const Matrix4& prevTfrm, Renderable* renderable)
    {
        const Matrix4& tfrmNoScale = renderable->GetMatrixNoScale();
        const UINT32 layer = Bitwise::MostSignificantBit(renderable->GetLayer());

        gPerObjectParamDef.gMatWorld.Set(buffer, tfrm);
        gPerObjectParamDef.gMatInvWorld.Set(buffer, tfrm.InverseAffine());
        gPerObjectParamDef.gMatWorldNoScale.Set(buffer, tfrmNoScale);
        gPerObjectParamDef.gMatInvWorldNoScale.Set(buffer, tfrmNoScale.InverseAffine());
        gPerObjectParamDef.gMatPrevWorld.Set(buffer, prevTfrm);
        gPerObjectParamDef.gLayer.Set(buffer, (INT32)layer);
        gPerObjectParamDef.gHasAnimation.Set(buffer, (UINT32)renderable->IsAnimated() ? 1 : 0);
        gPerObjectParamDef.gWriteVelocity.Set(buffer, (UINT32)renderable->GetWriteVelocity() ? 1 : 0);
        gPerObjectParamDef.gCastLights.Set(buffer, (UINT32)renderable->GetCastLights() ? 1 : 0);
    }

    void PerObjectBuffer::UpdatePerInstance(SPtr<GpuParamBlockBuffer>& perObjectBuffer, 
        SPtr<GpuParamBlockBuffer>& perInstanceBuffer, PerInstanceData* instanceData, UINT32 instanceCounter)
    {
        for (size_t i = 0; i < instanceCounter; i++)
            gPerInstanceParamDef.gInstances.Set(perInstanceBuffer, instanceData[i], (UINT32)i);
    }

    void PerObjectBuffer::UpdatePerMaterial(SPtr<GpuParamBlockBuffer>& perMaterialBuffer, const MaterialProperties& properties)
    {
        PerMaterialData data = ConvertMaterialProperties(properties);
        gPerMaterialParamDef.gMaterial.Set(perMaterialBuffer, data);
    }

    PerMaterialData PerObjectBuffer::ConvertMaterialProperties(const MaterialProperties& properties)
    {
        PerMaterialData data;
        data.BaseColor = properties.BaseColor.GetAsVector4();
        data.Metallic = properties.Metallic;
        data.Roughness = properties.Roughness;
        data.Reflectance = properties.Reflectance;
        data.AO = properties.AO;
        data.Emissive = properties.Emissive.GetAsVector4();
        data.UseIndirectLighting = (UINT32)properties.UseIndirectLighting ? 1 : 0;
        data.UseDiffuseIrrMap = (UINT32)properties.UseDiffuseIrrMap ? 1 : 0;
        data.UseSpecularIrrMap = (UINT32)properties.UseSpecularIrrMap ? 1 : 0;

        return data;
    }

    RenderableElement::RenderableElement(bool createPerMaterialBuffer)
        : RenderElement()
    {
        if(createPerMaterialBuffer)
            PerMaterialParamBuffer = gPerMaterialParamDef.CreateBuffer();
    }

    void RenderableElement::Draw() const
    {
        gRendererUtility().Draw(MeshElem, *SubMeshElem, InstanceCount);
    }

    RendererRenderable::RendererRenderable()
    {
        PerObjectParamBuffer = gPerObjectParamDef.CreateBuffer();
    }

    RendererRenderable::~RendererRenderable()
    { }

    void RendererRenderable::UpdatePerObjectBuffer()
    {
        PerObjectBuffer::UpdatePerObject(PerObjectParamBuffer, WorldTfrm, PrevWorldTfrm, RenderablePtr);
    }

    void RendererRenderable::UpdatePerInstanceBuffer(PerInstanceData* instanceData, UINT32 instanceCounter, UINT32 blockId)
    {
        PerObjectBuffer::UpdatePerInstance(PerObjectParamBuffer, gPerInstanceParamBuffer[blockId], instanceData, instanceCounter);
    }
}
