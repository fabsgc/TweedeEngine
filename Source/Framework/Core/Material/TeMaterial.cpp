#include "TeMaterial.h"
#include "TeShader.h"
#include "Resources/TeResourceHandle.h"
#include "Resources/TeResourceManager.h"

namespace te
{
    Material::Material()
    { }

    Material::~Material()
    { }

    Material::Material(const HShader& shader)
    {
        SetShader(shader.GetInternalPtr());
    }

    Material::Material(const SPtr<Shader>& shader)
    {
        SetShader(shader);
    }

    Material::Material(const HShader& shader, const Vector<SPtr<Technique>>& techniques)
    {
        _shader = shader.GetInternalPtr();
        _techniques = techniques;
    }

    Material::Material(const SPtr<Shader>& shader, const Vector<SPtr<Technique>>& techniques)
    {
        _shader = shader;
        _techniques = techniques;
    }

    void Material::Initialize()
    {
        InitializeTechniques();
    }

    void Material::InitializeTechniques()
    {
        _techniques.clear();

        if (_shader != nullptr)
        {
            _techniques = _shader->GetCompatibleTechniques();

            if (_techniques.empty())
                return;
        }

        InitializeGraphicsPipelineStates();
    }

    void Material::InitializeGraphicsPipelineStates()
    {
        _passesGpuParams.clear();

        UINT32 techniqueIdx = 0;
        for (auto& technique : _techniques)
        {
            for (UINT32 idx = 0; idx < technique->GetNumPasses(); idx++)
            {
                _passesGpuParams[techniqueIdx].push_back(technique->GetPass(idx)->GetGpuParams());
            }

            techniqueIdx++; 
        }
    }

    void Material::SetShader(const SPtr<Shader>& shader)
    {
        _shader = shader;
        InitializeTechniques();
    }

    UINT32 Material::GetNumPasses(UINT32 techniqueIdx) const
    {
        if (_shader == nullptr)
            return 0;

        if (techniqueIdx >= (UINT32)_techniques.size())
            return 0;

        return _techniques[techniqueIdx]->GetNumPasses();
    }

    SPtr<Pass> Material::GetPass(UINT32 passIdx, UINT32 techniqueIdx) const
    {
        if (_shader == nullptr)
            return nullptr;

        if (techniqueIdx >= (UINT32)_techniques.size())
            return nullptr;

        if (passIdx < 0 || passIdx >= _techniques[techniqueIdx]->GetNumPasses())
            return nullptr;

        return _techniques[techniqueIdx]->GetPass(passIdx);
    }

    /** Assigns a texture to the shader parameter with the specified name. */
    void Material::SetTexture(const String& name, const SPtr<Texture>& value, const TextureSurface& surface)
    {
        for (auto& technique : _passesGpuParams)
            for (auto& passParams : technique.second)
                passParams->SetTexture(name, value, surface);
    }

    /** Assigns a buffer to the shader parameter with the specified name. */
    void Material::SetBuffer(const String& name, const SPtr<GpuBuffer>& value)
    {
        for (auto& technique : _passesGpuParams)
            for (auto& passParams : technique.second)
                passParams->SetBuffer(name, value);
    }

    /** Assigns a sampler state to the shader parameter with the specified name. */
    void Material::SetSamplerState(const String& name, const SPtr<SamplerState>& value)
    {
        for (auto& technique : _passesGpuParams)
            for (auto& passParams : technique.second)
                passParams->SetSamplerState(name, value);
    }

    UINT32 Material::GetDefaultTechnique() const
    {
        return 0;
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

    void Material::FrameSync()
    { }
}
