#include "TeRendererRenderable.h"
#include "Mesh/TeMesh.h"
#include "Utility/TeBitwise.h"
#include "Renderer/TeRendererUtility.h"

namespace te
{ 
    PerInstanceParamDef gPerInstanceParamDef;
    PerMaterialParamDef gPerMaterialParamDef;
    PerObjectParamDef gPerObjectParamDef;
    PerCallParamDef gPerCallParamDef;

    void PerObjectBuffer::UpdatePerObject(SPtr<GpuParamBlockBuffer>& buffer, const Matrix4& tfrm,
        const Matrix4& prevTfrm, Renderable* renderable)
    {
        const Matrix4 tfrmNoScale = renderable->GetMatrixNoScale();
        const UINT32 layer = Bitwise::mostSignificantBit(renderable->GetLayer());

        gPerObjectParamDef.gMatWorld.Set(buffer, tfrm.Transpose());
        gPerObjectParamDef.gMatInvWorld.Set(buffer, tfrm.InverseAffine().Transpose());
        gPerObjectParamDef.gMatWorldNoScale.Set(buffer, tfrmNoScale.Transpose());
        gPerObjectParamDef.gMatInvWorldNoScale.Set(buffer, tfrmNoScale.InverseAffine().Transpose());
        gPerObjectParamDef.gMatPrevWorld.Set(buffer, prevTfrm.Transpose());
        gPerObjectParamDef.gLayer.Set(buffer, (INT32)layer);
    }

    void PerObjectBuffer::UpdatePerInstance(SPtr<GpuParamBlockBuffer>& perObjectBuffer, 
        SPtr<GpuParamBlockBuffer>& perInstanceBuffer, PerInstanceData* instanceData, UINT32 instanceCounter)
    {
        for (size_t i = 0; i < instanceCounter; i++)
            gPerInstanceParamDef.gInstances.Set(perInstanceBuffer, instanceData[i], (UINT32)i);
    }

    void PerObjectBuffer::UpdatePerMaterial(SPtr<GpuParamBlockBuffer>& perMaterialBuffer, const MaterialProperties& properties)
    {
        MaterialData data = ConvertMaterialProperties(properties);

        gPerMaterialParamDef.gDiffuse.Set(perMaterialBuffer, data.gDiffuse);
        gPerMaterialParamDef.gEmissive.Set(perMaterialBuffer, data.gEmissive);
        gPerMaterialParamDef.gSpecular.Set(perMaterialBuffer, data.gSpecular);
        gPerMaterialParamDef.gUseDiffuseMap.Set(perMaterialBuffer, data.gUseDiffuseMap);
        gPerMaterialParamDef.gUseNormalMap.Set(perMaterialBuffer, data.gUseNormalMap);
        gPerMaterialParamDef.gUseDepthMap.Set(perMaterialBuffer, data.gUseDepthMap);
        gPerMaterialParamDef.gUseSpecularMap.Set(perMaterialBuffer, data.gUseSpecularMap);
        gPerMaterialParamDef.gSpecularPower.Set(perMaterialBuffer, data.gSpecularPower);
    }

    MaterialData PerObjectBuffer::ConvertMaterialProperties(const MaterialProperties& properties)
    {
        MaterialData data;
        data.gDiffuse = properties.Diffuse.GetAsVector4();
        data.gEmissive = properties.Emissive.GetAsVector4();
        data.gSpecular = properties.Specular.GetAsVector4();
        data.gUseDiffuseMap = (UINT32)properties.UseDiffuseMap;
        data.gUseNormalMap = (UINT32)properties.UseNormalMap;
        data.gUseDepthMap = (UINT32)properties.UseDepthMap;
        data.gUseSpecularMap = (UINT32)properties.UseSpecularMap;
        data.gSpecularPower = properties.SpecularPower;

        return data;
    }

    RenderableElement::RenderableElement()
        : RenderElement()
    {
        PerMaterialParamBuffer = gPerMaterialParamDef.CreateBuffer();
    }

    void RenderableElement::Draw() const
    {
        gRendererUtility().Draw(MeshElem, SubMeshElem, InstanceCount);
    }

    RendererRenderable::RendererRenderable()
    {
        PerObjectParamBuffer = gPerObjectParamDef.CreateBuffer();
        PerCallParamBuffer = gPerCallParamDef.CreateBuffer();
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

    void RendererRenderable::UpdatePerCallBuffer(const Matrix4& viewProj, bool flush)
    {
        const Matrix4 worldViewProjMatrix = viewProj * RenderablePtr->GetMatrix();
        gPerCallParamDef.gMatWorldViewProj.Set(PerCallParamBuffer, worldViewProjMatrix.Transpose());

        //if (flush)
        //    PerCallParamBuffer->FlushToGPU(); TODO
    }
}
