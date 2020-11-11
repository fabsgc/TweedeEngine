#include "TeEditorUtils.h"

#include "Resources/TeBuiltinResources.h"
#include "Importer/TeTextureImportOptions.h"
#include "TeEditorResManager.h"
#include "TeCoreApplication.h"

namespace te
{
    void EditorUtils::ImportMeshMaterials(HMesh& mesh)
    {
        for (UINT32 i = 0; i < mesh->GetProperties().GetNumSubMeshes(); i++)
        {
            SubMesh& subMesh = mesh->GetProperties().GetSubMesh(i);
            MaterialProperties matProperties = subMesh.MatProperties;
            MaterialTextures matTextures = subMesh.MatTextures;

            if (!subMesh.Mat.GetHandleData())
            {
                HMaterial material = Material::Create(gBuiltinResources().GetBuiltinShader(BuiltinShader::Opaque));
                material->SetName(subMesh.MaterialName);
                material->SetSamplerState("AnisotropicSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Anisotropic));
                material->SetProperties(subMesh.MatProperties);

                const auto& BindTexture = [&](bool isSet, const String& textureName, const String& texturePath, HMaterial& material)
                {
                    auto textureImportOptions = TextureImportOptions::Create();
                    textureImportOptions->CpuCached = false;
                    textureImportOptions->GenerateMips = (textureName != "EmissiveMap") ? true : false;
                    textureImportOptions->MaxMip = 6;
                    textureImportOptions->Format = IsBigEndian() ? PF_RGBA8 : PF_BGRA8;

                    if (isSet)
                    {
                        HTexture texture = EditorResManager::Instance().Load<Texture>(texturePath, textureImportOptions);

                        if (texture.GetHandleData())
                        {
                            material->SetTexture(textureName, texture);
                            EditorResManager::Instance().Add<Texture>(texture);
                        }
                    }
                };

                BindTexture(matProperties.UseDiffuseMap, "DiffuseMap", matTextures.DiffuseMap, material);
                BindTexture(matProperties.UseEmissiveMap, "EmissiveMap", matTextures.EmissiveMap, material);
                BindTexture(matProperties.UseNormalMap, "NormalMap", matTextures.NormalMap, material);
                BindTexture(matProperties.UseSpecularMap, "SpecularMap", matTextures.SpecularMap, material);
                BindTexture(matProperties.UseBumpMap, "BumpMap", matTextures.BumpMap, material);
                BindTexture(matProperties.UseTransparencyMap, "TransparencyMap", matTextures.TransparencyMap, material);
                BindTexture(matProperties.UseReflectionMap, "ReflectionMap", matTextures.ReflectionMap, material);

                subMesh.Mat = material.GetNewHandleFromExisting();
                EditorResManager::Instance().Add<Material>(material);
            }
        }
    }

    void EditorUtils::GenerateViewportRenderTexture(RenderWindowData& renderData)
    {
        if (renderData.RenderTex)
            renderData.RenderTex = nullptr;
        if (renderData.ColorTex.GetHandleData())
            renderData.ColorTex.Release();
        if (renderData.DepthStencilTex.GetHandleData())
            renderData.DepthStencilTex.Release();

        renderData.TargetColorDesc.Type = TEX_TYPE_2D;
        renderData.TargetColorDesc.Width = renderData.Width;
        renderData.TargetColorDesc.Height = renderData.Height;
        renderData.TargetColorDesc.Format = PF_RGBA16F;
        renderData.TargetColorDesc.NumSamples = gCoreApplication().GetWindow()->GetDesc().MultisampleCount;
        renderData.TargetColorDesc.Usage = TU_RENDERTARGET;

        renderData.TargetDepthDesc.Type = TEX_TYPE_2D;
        renderData.TargetDepthDesc.Width = renderData.Width;
        renderData.TargetDepthDesc.Height = renderData.Height;
        renderData.TargetDepthDesc.Format = PF_RGBA8;
        renderData.TargetDepthDesc.NumSamples = gCoreApplication().GetWindow()->GetDesc().MultisampleCount;
        renderData.TargetDepthDesc.Usage = TU_DEPTHSTENCIL;

        renderData.ColorTex = Texture::Create(renderData.TargetColorDesc);
        renderData.DepthStencilTex = Texture::Create(renderData.TargetDepthDesc);

        renderData.RenderTexDesc.ColorSurfaces[0].Tex = renderData.ColorTex.GetInternalPtr();
        renderData.RenderTexDesc.ColorSurfaces[0].Face = 0;
        renderData.RenderTexDesc.ColorSurfaces[0].MipLevel = 0;

        renderData.RenderTexDesc.DepthStencilSurface.Tex = renderData.DepthStencilTex.GetInternalPtr();
        renderData.RenderTexDesc.DepthStencilSurface.Face = 0;
        renderData.RenderTexDesc.DepthStencilSurface.MipLevel = 0;

        renderData.RenderTex = RenderTexture::Create(renderData.RenderTexDesc);
    }

    /** Do a frustum culling on a light. Returns true if visible */
    bool EditorUtils::DoFrustumCulling(const HCamera& camera, const HLight& light)
    {
        return true;
    }

    /** Do a frustum culling on a scene camera. Returns true if visible */
    bool EditorUtils::DoFrustumCulling(const HCamera& camera, const HCamera& sceneCamera)
    {
        return true;
    }
}
