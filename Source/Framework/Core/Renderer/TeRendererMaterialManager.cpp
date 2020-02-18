#include "Renderer/TeRendererMaterialManager.h"
#include "Renderer/TeRendererMaterial.h"
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
#if TE_PLATFORM == TE_PLATFORM_WIN32 //TODO to remove when OpenGL will be done
            HShader shader = br.GetBuiltinShader(material.ShaderType);
            shaders.push_back(shader.GetInternalPtr());
#endif
        }

        InitMaterials(shaders);
    }

    RendererMaterialManager::~RendererMaterialManager()
    { 
        DestroyMaterials();
    }

    void RendererMaterialManager::_registerMaterial(RendererMaterialMetaData* metaData, BuiltinShader shaderType)
    {
        Vector<RendererMaterialData>& materials = GetMaterials();
        materials.push_back({ metaData, shaderType });
    }

    void RendererMaterialManager::InitMaterials(const Vector<SPtr<Shader>>& shaders)
    {
        Vector<RendererMaterialData>& materials = GetMaterials();
        for (UINT32 i = 0; i < materials.size(); i++)
        {
            materials[i].ShaderType = materials[i].ShaderType;
            materials[i].MetaData->ShaderElem = shaders[i];

            if (!shaders[i])
            {
                TE_DEBUG("Failed to load renderer material: {" + ToString((UINT32)materials[i].ShaderType) + "}", __FILE__, __LINE__);
                continue;
            }
        }
    }

    void RendererMaterialManager::DestroyMaterials()
    {
        Vector<RendererMaterialData>& materials = GetMaterials();
        for (UINT32 i = 0; i < materials.size(); i++)
        {
            materials[i].MetaData->ShaderElem = nullptr;
            te_delete(materials[i].MetaData->Instance);
        }
    }

    Vector<RendererMaterialData>& RendererMaterialManager::GetMaterials()
    {
        static Vector<RendererMaterialData> materials;
        return materials;
    }
}
