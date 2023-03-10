#include "Renderer/TeRendererMaterialManager.h"
#include "Renderer/TeRendererMaterial.h"
#include "Importer/TeShaderImportOptions.h"
#include "Resources/TeResourceManager.h"
#include "Material/TeShader.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(RendererMaterialManager)

    RendererMaterialManager::RendererMaterialManager()
    {
#if TE_PLATFORM == TE_PLATFORM_WIN32 // TODO to remove when OpenGL will be done
        BuiltinResources& br = BuiltinResources::Instance();

        // Note: Ideally I want to avoid loading all materials, and instead just load those that are used.
        Vector<RendererMaterialData>& materials = GetMaterials();
        Vector<SPtr<Shader>> shaders;

        for (auto& material : materials)
        {
            if (material.ShaderPath.type() == typeid(BuiltinShader))
            {
                HShader shader = br.GetBuiltinShader(std::any_cast<BuiltinShader>(material.ShaderPath));
                TE_ASSERT_ERROR(shader.IsLoaded(), "Shader not found")
                shaders.push_back(shader.GetInternalPtr());
            }
            else if (material.ShaderPath.type() == typeid(String))
            {
                auto shaderImportOptions = ShaderImportOptions::Create();
                HShader shader = gResourceManager().Load<Shader>(
                    std::any_cast<String>(material.ShaderPath), shaderImportOptions);
            }
            else
            {
                auto shaderImportOptions = ShaderImportOptions::Create();
                HShader shader = gResourceManager().Load<Shader>(
                    std::any_cast<const char*>(material.ShaderPath), shaderImportOptions);
            }
        }

        InitMaterials(shaders);
#endif
    }

    RendererMaterialManager::~RendererMaterialManager()
    { 
        DestroyMaterials();
    }

    void RendererMaterialManager::RegisterMaterial(RendererMaterialMetaData* metaData, const std::any& shaderPath)
    {
#if TE_PLATFORM == TE_PLATFORM_WIN32 // TODO to remove when OpenGL will be done
        const std::type_info& stringType = typeid(String);
        const std::type_info& charType = typeid(const char*);
        const std::type_info& builtinShaderType = typeid(BuiltinShader);
        const std::type_info& shaderType = shaderPath.type();

        if (shaderType != stringType && shaderType != charType && shaderType != builtinShaderType)
        {
            TE_ASSERT_ERROR(false, "ShaderPath must be a String or a BuiltinShader");
        }

        Vector<RendererMaterialData>& materials = GetMaterials();
        materials.push_back({ metaData, shaderPath });
#endif
    }

    void RendererMaterialManager::InitMaterials(const Vector<SPtr<Shader>>& shaders)
    {
        Vector<RendererMaterialData>& materials = GetMaterials();
        for (UINT32 i = 0; i < materials.size(); i++)
        {
#if TE_PLATFORM == TE_PLATFORM_WIN32 // TODO to remove when OpenGL will be done
            materials[i].ShaderPath = materials[i].ShaderPath;
            materials[i].MetaData->ShaderElem = shaders[i];

            if (!shaders[i])
            {
                if (materials[i].ShaderPath.type() == typeid(String))
                {
                    TE_DEBUG("Failed to load renderer material: {" + std::any_cast<String>(materials[i].ShaderPath) + "}");
                }
                else
                {
                    TE_DEBUG("Failed to load renderer material: {" + ToString(std::any_cast<UINT32>(materials[i].ShaderPath)) + "}");
                }

                continue;
            }
#endif
        }
    }

    void RendererMaterialManager::DestroyMaterials()
    {
        Vector<RendererMaterialData>& materials = GetMaterials();
        for (UINT32 i = 0; i < materials.size(); i++)
        {
#if TE_PLATFORM == TE_PLATFORM_WIN32 // TODO to remove when OpenGL will be done
            materials[i].MetaData->ShaderElem = nullptr;

            for (auto& entry : materials[i].MetaData->Instances)
            {
                if(entry != nullptr)
                    te_delete(entry);
            }

            materials[i].MetaData->Instances.clear();
#endif
        }
    }

    Vector<RendererMaterialData>& RendererMaterialManager::GetMaterials()
    {
        static Vector<RendererMaterialData> materials;
        return materials;
    }
}
