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

        gPerMaterialParamDef.gAmbient.Set(perMaterialBuffer, data.gAmbient);
        gPerMaterialParamDef.gDiffuse.Set(perMaterialBuffer, data.gDiffuse);
        gPerMaterialParamDef.gEmissive.Set(perMaterialBuffer, data.gEmissive);
        gPerMaterialParamDef.gSpecular.Set(perMaterialBuffer, data.gSpecular);
        gPerMaterialParamDef.gUseDiffuseMap.Set(perMaterialBuffer, data.gUseDiffuseMap);
        gPerMaterialParamDef.gUseEmissiveMap.Set(perMaterialBuffer, data.gUseEmissiveMap);
        gPerMaterialParamDef.gUseNormalMap.Set(perMaterialBuffer, data.gUseNormalMap);
        gPerMaterialParamDef.gUseSpecularMap.Set(perMaterialBuffer, data.gUseSpecularMap);
        gPerMaterialParamDef.gUseBumpMap.Set(perMaterialBuffer, data.gUseBumpMap);
        gPerMaterialParamDef.gUseParallaxMap.Set(perMaterialBuffer, data.gUseParallaxMap);
        gPerMaterialParamDef.gUseTransparencyMap.Set(perMaterialBuffer, data.gUseTransparencyMap);
        gPerMaterialParamDef.gUseReflectionMap.Set(perMaterialBuffer, data.gUseReflectionMap);
        gPerMaterialParamDef.gUseOcclusionMap.Set(perMaterialBuffer, data.gUseOcclusionMap);
        gPerMaterialParamDef.gSpecularPower.Set(perMaterialBuffer, data.gSpecularPower);
        gPerMaterialParamDef.gTransparency.Set(perMaterialBuffer, data.gTransparency);
        gPerMaterialParamDef.gIndexOfRefraction.Set(perMaterialBuffer, data.gIndexOfRefraction);
        gPerMaterialParamDef.gReflection.Set(perMaterialBuffer, data.gReflection);
        gPerMaterialParamDef.gAbsorbance.Set(perMaterialBuffer, data.gAbsorbance);
        gPerMaterialParamDef.gBumpScale.Set(perMaterialBuffer, data.gBumpScale);
        gPerMaterialParamDef.gAlphaThreshold.Set(perMaterialBuffer, data.gAlphaThreshold);
    }

    MaterialData PerObjectBuffer::ConvertMaterialProperties(const MaterialProperties& properties)
    {
        MaterialData data;
        data.gAmbient = properties.Ambient.GetAsVector4();
        data.gDiffuse = properties.Diffuse.GetAsVector4();
        data.gEmissive = properties.Emissive.GetAsVector4();
        data.gSpecular = properties.Specular.GetAsVector4();
        data.gUseDiffuseMap = (UINT32)properties.UseDiffuseMap ? 1 : 0;
        data.gUseEmissiveMap = (UINT32)properties.UseEmissiveMap ? 1 : 0;
        data.gUseNormalMap = (UINT32)properties.UseNormalMap ? 1 : 0;
        data.gUseSpecularMap = (UINT32)properties.UseSpecularMap ? 1 : 0;
        data.gUseBumpMap = (UINT32)properties.UseBumpMap ? 1 : 0;
        data.gUseParallaxMap = (UINT32)properties.UseParallaxMap ? 1 : 0;
        data.gUseTransparencyMap = (UINT32)properties.UseTransparencyMap ? 1 : 0;
        data.gUseReflectionMap = (UINT32)properties.UseReflectionMap ? 1 : 0;
        data.gUseOcclusionMap = (UINT32)properties.UseOcclusionMap ? 1 : 0;
        data.gSpecularPower = properties.SpecularPower;
        data.gTransparency = properties.Transparency;
        data.gIndexOfRefraction = properties.IndexOfRefraction;
        data.gReflection = properties.Reflection;
        data.gAbsorbance = properties.Absorbance;
        data.gBumpScale = properties.BumpScale;
        data.gAlphaThreshold = properties.AlphaThreshold;
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
        // TODO is instance, it does not work
        // const Matrix4 worldViewProjMatrix = viewProj * RenderablePtr->GetMatrixNoScale();
        // gPerCallParamDef.gMatWorldViewProj.Set(PerCallParamBuffer, worldViewProjMatrix.Transpose());

        //if (flush)
        //    PerCallParamBuffer->FlushToGPU();
    }
}
