#include "TeMaterial.h"
#include "TeShader.h"
#include "Resources/TeResourceHandle.h"
#include "Resources/TeResourceManager.h"

namespace te
{
    Material::Material()
    { }

    Material::Material(const HShader& shader)
    {
        SetShader(shader.GetInternalPtr());
    }

    Material::Material(const SPtr<Shader>& shader)
    {
        SetShader(shader);
    }

    void Material::Initialize()
    { }

    void Material::SetShader(const SPtr<Shader>& shader)
    {
        _shader = shader;
    }

    HMaterial Material::Create()
    {
        const SPtr<Material> materialPtr = CreateEmpty();
        materialPtr->Initialize();

        return static_resource_cast<Material>(gResourceManager()._createResourceHandle(materialPtr));
    }

    HMaterial Material::Create(const HShader& shader)
    {
        SPtr<Material> materialPtr = te_core_ptr<Material>(new (te_allocate<Material>()) Material(shader));
        materialPtr->SetThisPtr(materialPtr);
        materialPtr->Initialize();

        return static_resource_cast<Material>(gResourceManager()._createResourceHandle(materialPtr));
    }

    HMaterial Material::Create(const SPtr<Shader>& shader)
    {
        SPtr<Material> materialPtr = te_core_ptr<Material>(new (te_allocate<Material>()) Material(shader));
        materialPtr->SetThisPtr(materialPtr);
        materialPtr->Initialize();

        return static_resource_cast<Material>(gResourceManager()._createResourceHandle(materialPtr));
    }

    SPtr<Material> Material::CreateEmpty()
    {
        SPtr<Material> newMat = te_core_ptr<Material>(new (te_allocate<Material>()) Material());
        newMat->SetThisPtr(newMat);

        return newMat;
    }

    void Material::_markCoreDirty(MaterialDirtyFlags flags)
    {
        MarkCoreDirty((UINT32)flags);
    }
}
