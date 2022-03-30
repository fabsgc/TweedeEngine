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
        data.Occlusion = properties.Occlusion;
        data.Emissive = properties.Emissive.GetAsVector4();
        data.SheenColor = properties.SheenColor.GetAsVector4();
        data.SheenRoughness = properties.SheenRoughness;
        data.ClearCoat = properties.ClearCoat;
        data.ClearCoatRoughness = properties.ClearCoatRoughness;
        data.SubsurfaceColor = properties.SubsurfaceColor.GetAsVector4();
        data.SurSurfacePower = properties.SubsurfacePower;
        data.Anisotropy = properties.Anisotropy;
        data.AnisotropyDirection = properties.AnisotropyDirection;
        data.AlphaTreshold = properties.AlphaThreshold;
        data.UV0Repeat = properties.TextureRepeat;
        data.UV0Offset = properties.TextureOffset;
        data.ParallaxScale = properties.ParallaxScale;
        data.ParallaxSamples = properties.ParallaxSamples;
        data.MicroThickness = properties.MicroThickness;
        data.Thickness = properties.Thickness;
        data.Transmission = properties.Transmission;
        data.Absorption = properties.Absorption;
        data.RefractionType = (UINT32)properties.RefractType;
        data.UseBaseColorMap = (UINT32)properties.UseBaseColorMap;
        data.UseMetallicMap = (UINT32)properties.UseMetallicMap;
        data.UseRoughnessMap = (UINT32)properties.UseRoughnessMap;
        data.UseMetallicRoughnessMap = (UINT32)properties.UseMetallicRoughnessMap;
        data.UseReflectanceMap = (UINT32)properties.UseReflectanceMap;
        data.UseOcclusionMap = (UINT32)properties.UseOcclusionMap;
        data.UseEmissiveMap = (UINT32)properties.UseEmissiveMap;
        data.UseSheenColorMap = (UINT32)properties.UseSheenColorMap;
        data.UseSheenRoughnessMap = (UINT32)properties.UseSheenRoughnessMap;
        data.UseClearCoatMap = (UINT32)properties.UseClearCoatMap;
        data.UseClearCoatRoughnessMap = (UINT32)properties.UseClearCoatRoughnessMap;
        data.UseClearCoatNormalMap = (UINT32)properties.UseClearCoatNormalMap;
        data.UseNormalMap = (UINT32)properties.UseNormalMap;
        data.UseParallaxMap = (UINT32)properties.UseParallaxMap;
        data.UseTransmissionMap = (UINT32)properties.UseTransmissionMap;
        data.UseOpacityMap = (UINT32)properties.UseOpacityMap;
        data.UseAnisotropyDirectionMap = (UINT32)properties.UseAnisotropyDirectionMap;
        data.DoIndirectLighting = (UINT32)properties.DoIndirectLighting;

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
