#include "TeEditorUtils.h"

#include "TeCoreApplication.h"
#include "RenderAPI/TeSubMesh.h"
#include "Threading/TeTaskScheduler.h"
#include "Resources/TeBuiltinResources.h"
#include "Importer/TeTextureImportOptions.h"
#include "Components/TeCCamera.h"
#include "Components/TeCLight.h"
#include "Components/TeCRenderable.h"
#include "Components/TeCAudioListener.h"
#include "Components/TeCAudioSource.h"
#include "Components/TeCRigidBody.h"
#include "Components/TeCSoftBody.h"
#include "TeEditorResManager.h"
#include "Image/TeTexture.h"
#include "Image/TePixelUtil.h"
#include "Mesh/TeMesh.h"

namespace te
{
    const String EditorUtils::DELETE_BINDING = "Delete";
    const String EditorUtils::COPY_BINDING = "Copy";
    const String EditorUtils::PASTE_BINDING = "Paste";

    void EditorUtils::ImportMeshMaterials(HMesh& mesh, bool SRGB)
    {
        UnorderedMap<String, HMaterial> createdMaterials;
        bool notAllLoaded = false;
        List<SPtr<Task>> tasks;

        const auto& BindTexture = [&](bool* isSet, const String& textureName, const String& texturePath, HMaterial& material, bool SRGB)
        {
            if (*isSet)
            {
                auto textureImportOptions = TextureImportOptions::Create();
                textureImportOptions->CpuCached = false;
                textureImportOptions->GenerateMips = true;
                textureImportOptions->MipMapsPreserveCoverage = true;
                textureImportOptions->MaxMip = 0;
                textureImportOptions->Format = PixelUtil::BestFormatFromFile(texturePath);
                textureImportOptions->SRGB = SRGB;

                HTexture texture = EditorResManager::Instance().Load<Texture>(texturePath, textureImportOptions);

                if (texture.IsLoaded())
                {
                    material->SetTexture(textureName, texture);
                    EditorResManager::Instance().Add<Texture>(texture);
                }
                else
                {
                    *isSet = false;
                }
            }
        };

        for (UINT32 i = 0; i < mesh->GetProperties().GetNumSubMeshes(); i++)
        {
            SubMesh& subMesh = mesh->GetProperties().GetSubMesh(i);

            if (createdMaterials.find(subMesh.MaterialName) != createdMaterials.end())
            {
                subMesh.Mat = createdMaterials[subMesh.MaterialName].GetNewHandleFromExisting();

                subMesh.MatProperties.UseBaseColorMap = subMesh.Mat->GetTexture("BaseColorMap") != nullptr;
                subMesh.MatProperties.UseMetallicMap = subMesh.Mat->GetTexture("MetallicMap") != nullptr;
                subMesh.MatProperties.UseRoughnessMap = subMesh.Mat->GetTexture("RoughnessMap") != nullptr;
                subMesh.MatProperties.UseOcclusionMap = subMesh.Mat->GetTexture("OcclusionMap") != nullptr;
                subMesh.MatProperties.UseEmissiveMap = subMesh.Mat->GetTexture("EmissiveMap") != nullptr;
                subMesh.MatProperties.UseSheenColorMap = subMesh.Mat->GetTexture("SheenColorMap") != nullptr;
                subMesh.MatProperties.UseSheenRoughnessMap = subMesh.Mat->GetTexture("SheenRoughnessMap") != nullptr;
                subMesh.MatProperties.UseClearCoatMap = subMesh.Mat->GetTexture("ClearCoatMap") != nullptr;
                subMesh.MatProperties.UseClearCoatRoughnessMap = subMesh.Mat->GetTexture("ClearCoatRoughnessMap") != nullptr;
                subMesh.MatProperties.UseClearCoatNormalMap = subMesh.Mat->GetTexture("ClearCoatNormalMap") != nullptr;
                subMesh.MatProperties.UseNormalMap = subMesh.Mat->GetTexture("NormalMap") != nullptr;
                subMesh.MatProperties.UseParallaxMap = subMesh.Mat->GetTexture("ParallaxMap") != nullptr;
                subMesh.MatProperties.UseTransmissionMap = subMesh.Mat->GetTexture("TransmissionMap") != nullptr;
                subMesh.MatProperties.UseAnisotropyDirectionMap = subMesh.Mat->GetTexture("AnisotropyDirectionMap") != nullptr;
            }
            else if (!subMesh.Mat.IsLoaded())
            {
                HMaterial material = Material::Create(gBuiltinResources().GetBuiltinShader(BuiltinShader::Opaque));
                material->SetName(subMesh.MaterialName);
                material->SetProperties(subMesh.MatProperties);

                subMesh.Mat = material.GetNewHandleFromExisting();
                EditorResManager::Instance().Add<Material>(material);
                createdMaterials[subMesh.MaterialName] = material.GetNewHandleFromExisting();

                if (subMesh.MatProperties.UseBaseColorMap)
                {
                    tasks.push_back(Task::Create(subMesh.MaterialName,
                        [&]() { BindTexture(&subMesh.MatProperties.UseBaseColorMap, "BaseColorMap", subMesh.MatTextures.BaseColorMap, createdMaterials[subMesh.MaterialName], SRGB); }));
                }
                if (subMesh.MatProperties.UseMetallicMap)
                {
                    tasks.push_back(Task::Create(subMesh.MaterialName,
                        [&]() { BindTexture(&subMesh.MatProperties.UseMetallicMap, "MetallicMap", subMesh.MatTextures.MetallicMap, createdMaterials[subMesh.MaterialName], false); }));
                }
                if (subMesh.MatProperties.UseRoughnessMap)
                {
                    tasks.push_back(Task::Create(subMesh.MaterialName,
                        [&]() { BindTexture(&subMesh.MatProperties.UseRoughnessMap, "RoughnesMap", subMesh.MatTextures.RoughnessMap, createdMaterials[subMesh.MaterialName], false); }));
                }
                if (subMesh.MatProperties.UseReflectanceMap)
                {
                    tasks.push_back(Task::Create(subMesh.MaterialName,
                        [&]() { BindTexture(&subMesh.MatProperties.UseReflectanceMap, "ReflectanceMap", subMesh.MatTextures.ReflectanceMap, createdMaterials[subMesh.MaterialName], false); }));
                }
                if (subMesh.MatProperties.UseOcclusionMap)
                {
                    tasks.push_back(Task::Create(subMesh.MaterialName,
                        [&]() { BindTexture(&subMesh.MatProperties.UseOcclusionMap, "OcclusionMap", subMesh.MatTextures.OcclusionMap, createdMaterials[subMesh.MaterialName], false); }));
                }
                if (subMesh.MatProperties.UseEmissiveMap)
                {
                    tasks.push_back(Task::Create(subMesh.MaterialName,
                        [&]() { BindTexture(&subMesh.MatProperties.UseEmissiveMap, "EmissiveMap", subMesh.MatTextures.EmissiveMap, createdMaterials[subMesh.MaterialName], SRGB); }));
                }
                if (subMesh.MatProperties.UseSheenColorMap)
                {
                    tasks.push_back(Task::Create(subMesh.MaterialName,
                        [&]() { BindTexture(&subMesh.MatProperties.UseSheenColorMap, "SheenColorMap", subMesh.MatTextures.SheenColorMap, createdMaterials[subMesh.MaterialName], SRGB); }));
                }
                if (subMesh.MatProperties.UseSheenRoughnessMap)
                {
                    tasks.push_back(Task::Create(subMesh.MaterialName,
                        [&]() { BindTexture(&subMesh.MatProperties.UseSheenRoughnessMap, "SheenRoughnessMap", subMesh.MatTextures.SheenRoughnessMap, createdMaterials[subMesh.MaterialName], false); }));
                }
                if (subMesh.MatProperties.UseClearCoatMap)
                {
                    tasks.push_back(Task::Create(subMesh.MaterialName,
                        [&]() { BindTexture(&subMesh.MatProperties.UseClearCoatMap, "ClearCoatMap", subMesh.MatTextures.ClearCoatMap, createdMaterials[subMesh.MaterialName], false); }));
                }
                if (subMesh.MatProperties.UseClearCoatRoughnessMap)
                {
                    tasks.push_back(Task::Create(subMesh.MaterialName,
                        [&]() { BindTexture(&subMesh.MatProperties.UseClearCoatRoughnessMap, "ClearCoatRoughnessMap", subMesh.MatTextures.ClearCoatRoughnessMap, createdMaterials[subMesh.MaterialName], false); }));
                }
                if (subMesh.MatProperties.UseClearCoatNormalMap)
                {
                    tasks.push_back(Task::Create(subMesh.MaterialName,
                        [&]() { BindTexture(&subMesh.MatProperties.UseClearCoatNormalMap, "ClearCoatNormalMap", subMesh.MatTextures.ClearCoatNormalMap, createdMaterials[subMesh.MaterialName], false); }));
                }
                if (subMesh.MatProperties.UseNormalMap)
                {
                    tasks.push_back(Task::Create(subMesh.MaterialName,
                        [&]() { BindTexture(&subMesh.MatProperties.UseNormalMap, "NormalMap", subMesh.MatTextures.NormalMap, createdMaterials[subMesh.MaterialName], false); }));
                }
                if (subMesh.MatProperties.UseParallaxMap)
                {
                    tasks.push_back(Task::Create(subMesh.MaterialName,
                        [&]() { BindTexture(&subMesh.MatProperties.UseParallaxMap, "ParallaxMap", subMesh.MatTextures.ParallaxMap, createdMaterials[subMesh.MaterialName], false); }));
                }
                if (subMesh.MatProperties.UseTransmissionMap)
                {
                    tasks.push_back(Task::Create(subMesh.MaterialName,
                        [&]() { BindTexture(&subMesh.MatProperties.UseTransmissionMap, "TransmissionMap", subMesh.MatTextures.TransmissionMap, createdMaterials[subMesh.MaterialName], false); }));
                }
                if (subMesh.MatProperties.UseAnisotropyDirectionMap)
                {
                    tasks.push_back(Task::Create(subMesh.MaterialName,
                        [&]() { BindTexture(&subMesh.MatProperties.UseAnisotropyDirectionMap, "AnisotropyDirectionMap", subMesh.MatTextures.AnisotropyDirectionMap, createdMaterials[subMesh.MaterialName], false); }));
                }
            }
        }

        for (auto& task : tasks)
        {
            gTaskScheduler().AddTask(task);
        }

        do
        {
            notAllLoaded = false;
            for (auto task : tasks)
            {
                if (!task->IsComplete())
                    notAllLoaded = true;
            }
        } while (notAllLoaded);

        for (UINT32 i = 0; i < mesh->GetProperties().GetNumSubMeshes(); i++)
        {
            SubMesh& subMesh = mesh->GetProperties().GetSubMesh(i);
            subMesh.Mat->SetProperties(subMesh.MatProperties);
        }
    }
}
