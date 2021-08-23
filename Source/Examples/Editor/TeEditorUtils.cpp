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
#include "Mesh/TeMesh.h"

namespace te
{
    const String EditorUtils::DELETE_BINDING = "Delete";
    const String EditorUtils::COPY_BINDING = "Copy";
    const String EditorUtils::PASTE_BINDING = "Paste";

    void EditorUtils::ImportMeshMaterials(HMesh& mesh)
    {
        Map<String, HMaterial> createdMaterials;
        bool notAllLoaded = false;
        List<SPtr<Task>> tasks;

        const auto& BindTexture = [&](bool* isSet, const String& textureName, const String& texturePath, HMaterial& material)
        {
            if (*isSet)
            {
                auto textureImportOptions = TextureImportOptions::Create();
                textureImportOptions->CpuCached = false;
                textureImportOptions->GenerateMips = true;
                textureImportOptions->MaxMip = 0;
                textureImportOptions->Format = Util::IsBigEndian() ? PF_RGBA8 : PF_BGRA8;

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

                subMesh.MatProperties.UseDiffuseMap = subMesh.Mat->GetTexture("DiffuseMap") != nullptr;
                subMesh.MatProperties.UseEmissiveMap = subMesh.Mat->GetTexture("EmissiveMap") != nullptr;
                subMesh.MatProperties.UseNormalMap = subMesh.Mat->GetTexture("NormalMap") != nullptr;
                subMesh.MatProperties.UseSpecularMap = subMesh.Mat->GetTexture("SpecularMap") != nullptr;
                subMesh.MatProperties.UseBumpMap = subMesh.Mat->GetTexture("BumpMap") != nullptr;
                subMesh.MatProperties.UseTransparencyMap = subMesh.Mat->GetTexture("TransparencyMap") != nullptr;
                subMesh.MatProperties.UseReflectionMap = subMesh.Mat->GetTexture("ReflectionMap") != nullptr;
            }
            else if (!subMesh.Mat.IsLoaded())
            {
                HMaterial material = Material::Create(gBuiltinResources().GetBuiltinShader(BuiltinShader::Opaque));
                material->SetName(subMesh.MaterialName);
                material->SetSamplerState("AnisotropicSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Anisotropic));
                material->SetProperties(subMesh.MatProperties);

                subMesh.Mat = material.GetNewHandleFromExisting();
                EditorResManager::Instance().Add<Material>(material);
                createdMaterials[subMesh.MaterialName] = material.GetNewHandleFromExisting();

                if (subMesh.MatProperties.UseDiffuseMap)
                {
                    tasks.push_back(Task::Create(subMesh.MaterialName,
                        [&]() { BindTexture(&subMesh.MatProperties.UseDiffuseMap, "DiffuseMap", subMesh.MatTextures.DiffuseMap, createdMaterials[subMesh.MaterialName]); }));
                }
                if (subMesh.MatProperties.UseEmissiveMap)
                {
                    tasks.push_back(Task::Create(subMesh.MaterialName,
                        [&]() { BindTexture(&subMesh.MatProperties.UseEmissiveMap, "EmissiveMap", subMesh.MatTextures.EmissiveMap, createdMaterials[subMesh.MaterialName]); }));
                }
                if (subMesh.MatProperties.UseNormalMap)
                {
                    tasks.push_back(Task::Create(subMesh.MaterialName,
                        [&]() { BindTexture(&subMesh.MatProperties.UseNormalMap, "NormalMap", subMesh.MatTextures.NormalMap, createdMaterials[subMesh.MaterialName]); }));
                }
                if (subMesh.MatProperties.UseSpecularMap)
                {
                    tasks.push_back(Task::Create(subMesh.MaterialName,
                        [&]() { BindTexture(&subMesh.MatProperties.UseSpecularMap, "SpecularMap", subMesh.MatTextures.SpecularMap, createdMaterials[subMesh.MaterialName]); }));
                }
                if (subMesh.MatProperties.UseBumpMap)
                {
                    tasks.push_back(Task::Create(subMesh.MaterialName,
                        [&]() { BindTexture(&subMesh.MatProperties.UseBumpMap, "BumpMap", subMesh.MatTextures.BumpMap, createdMaterials[subMesh.MaterialName]); }));
                }
                if (subMesh.MatProperties.UseTransparencyMap)
                {
                    tasks.push_back(Task::Create(subMesh.MaterialName,
                        [&]() { BindTexture(&subMesh.MatProperties.UseTransparencyMap, "TransparencyMap", subMesh.MatTextures.TransparencyMap, createdMaterials[subMesh.MaterialName]); }));
                }
                if (subMesh.MatProperties.UseReflectionMap)
                {
                    tasks.push_back(Task::Create(subMesh.MaterialName,
                        [&]() { BindTexture(&subMesh.MatProperties.UseReflectionMap, "ReflectionMap", subMesh.MatTextures.ReflectionMap, createdMaterials[subMesh.MaterialName]); }));
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
