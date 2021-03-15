#include "TeEditorUtils.h"

#include "Resources/TeBuiltinResources.h"
#include "Importer/TeTextureImportOptions.h"
#include "TeEditorResManager.h"
#include "TeCoreApplication.h"

namespace te
{
    const String EditorUtils::DELETE_BINDING = "Delete";
    const String EditorUtils::COPY_BINDING = "Copy";
    const String EditorUtils::PASTE_BINDING = "Paste";

    void EditorUtils::ImportMeshMaterials(HMesh& mesh)
    {
        for (UINT32 i = 0; i < mesh->GetProperties().GetNumSubMeshes(); i++)
        {
            SubMesh& subMesh = mesh->GetProperties().GetSubMesh(i);
            MaterialProperties matProperties = subMesh.MatProperties;
            MaterialTextures matTextures = subMesh.MatTextures;

            if (!subMesh.Mat.IsLoaded())
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
                    textureImportOptions->MaxMip = 8;
                    textureImportOptions->Format = Util::IsBigEndian() ? PF_RGBA8 : PF_BGRA8;

                    if (isSet)
                    {
                        HTexture texture = EditorResManager::Instance().Load<Texture>(texturePath, textureImportOptions);

                        if (texture.IsLoaded())
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
        if (renderData.ColorTex.IsLoaded())
            renderData.ColorTex.Release();
        if (renderData.DepthStencilTex.IsLoaded())
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

    bool EditorUtils::DoFrustumCulling(const HCamera& camera, const SPtr<CRenderable> renderable)
    {
        ConvexVolume worldFrustum = camera->GetWorldFrustum();
        const Vector3& worldCameraPosition = camera->GetTransform().GetPosition();
        float baseCullDistance = camera->GetRenderSettings()->CullDistance;

        Bounds boundaries = renderable->GetBounds();
        const Sphere& boundingSphere = boundaries.GetSphere();
        const Vector3& worldRenderablePosition = boundingSphere.GetCenter();

        float distanceToCameraSq = worldCameraPosition.SquaredDistance(worldRenderablePosition);
        float correctedCullDistance = renderable->GetCullDistanceFactor() * baseCullDistance;
        float maxDistanceToCamera = correctedCullDistance + boundingSphere.GetRadius();

        if (distanceToCameraSq > maxDistanceToCamera* maxDistanceToCamera)
            return false;

        if (worldFrustum.Intersects(boundingSphere))
        {
            const AABox& boundingBox = boundaries.GetBox();
            if (worldFrustum.Intersects(boundingBox))
                return true;
        }

        return false;
    }

    bool EditorUtils::DoFrustumCulling(const HCamera& camera, const SPtr<CLight> light)
    {
        static float cullDistanceFactor = 1.0f;
        Sphere boundingSphere = light->GetBounds();

        return DoFrustumCulling(camera, boundingSphere, cullDistanceFactor);
    }

    bool EditorUtils::DoFrustumCulling(const HCamera& camera, const SPtr<CAudioListener> audio)
    {
        static float cullDistanceFactor = 1.0f;

        Sphere boundingSphere;
        boundingSphere.SetCenter(audio->GetTransform().GetPosition());
        boundingSphere.SetRadius(1.0f);

        return DoFrustumCulling(camera, boundingSphere, cullDistanceFactor);
    }

    bool EditorUtils::DoFrustumCulling(const HCamera& camera, const SPtr<CAudioSource> audio)
    {
        static float cullDistanceFactor = 1.0f;

        Sphere boundingSphere;
        boundingSphere.SetCenter(audio->GetTransform().GetPosition());
        boundingSphere.SetRadius(1.0f);

        return DoFrustumCulling(camera, boundingSphere, cullDistanceFactor);
    }

    bool EditorUtils::DoFrustumCulling(const HCamera& camera, const SPtr<CCamera> sceneCamera)
    {
        static float cullDistanceFactor = 1.0f;

        Sphere boundingSphere;
        boundingSphere.SetCenter(sceneCamera->GetTransform().GetPosition());
        boundingSphere.SetRadius(1.0f);

        return DoFrustumCulling(camera, boundingSphere, cullDistanceFactor);
    }

    bool EditorUtils::DoFrustumCulling(const HCamera& camera, const Sphere& boundingSphere, const float& cullDistanceFactor)
    {
        ConvexVolume worldFrustum = camera->GetWorldFrustum();
        const Vector3& worldCameraPosition = camera->GetTransform().GetPosition();
        float baseCullDistance = camera->GetRenderSettings()->CullDistance;

        const Vector3& worldRenderablePosition = boundingSphere.GetCenter();

        float distanceToCameraSq = worldCameraPosition.SquaredDistance(worldRenderablePosition);
        float correctedCullDistance = cullDistanceFactor * baseCullDistance;
        float maxDistanceToCamera = correctedCullDistance + boundingSphere.GetRadius();

        if (distanceToCameraSq > maxDistanceToCamera* maxDistanceToCamera)
            return false;

        if (worldFrustum.Intersects(boundingSphere))
            return true;

        if (worldFrustum.Contains(boundingSphere.GetCenter()))
            return true;

        return false;
    }
}
