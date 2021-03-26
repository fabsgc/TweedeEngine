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
        BuiltinResources& br = BuiltinResources::Instance();

        // Note: Ideally I want to avoid loading all materials, and instead just load those that are used.
        Vector<RendererMaterialData>& materials = GetMaterials();
        Vector<SPtr<Shader>> shaders;
        for (auto& material : materials)
        {
#if TE_PLATFORM == TE_PLATFORM_WIN32 // TODO to remove when OpenGL will be done
            if (material.ShaderPath.type() == typeid(BuiltinShader))
            {
                HShader shader = br.GetBuiltinShader(std::any_cast<BuiltinShader>(material.ShaderPath));
                shaders.push_back(shader.GetInternalPtr());
            }
            else
            {
                auto shaderImportOptions = ShaderImportOptions::Create();
                HShader shader = gResourceManager().Load<Shader>(
                    std::any_cast<String>(material.ShaderPath), shaderImportOptions);
            }
#endif
        }

        InitMaterials(shaders);
    }

    RendererMaterialManager::~RendererMaterialManager()
    { 
        DestroyMaterials();
    }

    void RendererMaterialManager::_registerMaterial(RendererMaterialMetaData* metaData, const std::any& shaderPath)
    {
#if TE_PLATFORM == TE_PLATFORM_WIN32 // TODO to remove when OpenGL will be done
        const std::type_info& stringType = typeid(String);
        const std::type_info& builtinShaderType = typeid(BuiltinShader);
        const std::type_info& shaderType = shaderPath.type();

        if (shaderType != stringType && shaderType != builtinShaderType)
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
            if(materials[i].MetaData->Instance)
                te_delete(materials[i].MetaData->Instance);
#endif
        }
    }

    Vector<RendererMaterialData>& RendererMaterialManager::GetMaterials()
    {
        static Vector<RendererMaterialData> materials;
        return materials;
    }
}
