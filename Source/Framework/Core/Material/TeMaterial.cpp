#include "TeMaterial.h"
#include "TeShader.h"
#include "Resources/TeResourceHandle.h"
#include "Resources/TeResourceManager.h"

namespace te
{
    Material::Material()
    { }

    Material::~Material()
    {
        for (auto& param : _params)
        {
            if(param.second.Param)
                te_deallocate(param.second.Param); //I's sure that types here are primitive or very simple type sush as Vector3
        }
    }

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
        CoreObject::Initialize();
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
    }

    void Material::CreateGpuParams(UINT32 techniqueIdx, Vector<SPtr<GpuParams>>& outputParams)
    {
        outputParams.clear();
        SPtr<Technique> currentTechnique = _techniques[techniqueIdx];

        if (!currentTechnique)
            return;

        for (UINT32 idx = 0; idx < currentTechnique->GetNumPasses(); idx++)
        {
            SPtr<GraphicsPipelineState> graphicPipelineState = _techniques[techniqueIdx]->GetPass(idx)->GetGraphicsPipelineState();
            outputParams.push_back(GpuParams::Create(graphicPipelineState));

            for (auto& texture : _textures)
                outputParams[idx]->SetTexture(texture.first, texture.second->TextureElem, texture.second->TextureSurfaceElem);

            for (auto& samplerState : _samplerStates)
                outputParams[idx]->SetSamplerState(samplerState.first, samplerState.second);

            for (auto& buffer : _buffers)
                outputParams[idx]->SetBuffer(buffer.first, buffer.second);

            for (auto& param : _params)
                outputParams[idx]->SetParam(param.first, param.second.Param, (UINT32)param.second.Size);
        }
    }

    void Material::SetGpuParam(SPtr<GpuParams> outparams)
    {
        for (auto& param : _params)
            outparams->SetParam(param.first, param.second.Param, (UINT32)param.second.Size);
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

        if (passIdx >= _techniques[techniqueIdx]->GetNumPasses())
            return nullptr;

        return _techniques[techniqueIdx]->GetPass(passIdx);
    }

    /** Assigns a texture to the shader parameter with the specified name. */
    void Material::SetTexture(const String& name, const SPtr<Texture>& value, const TextureSurface& surface)
    {
        if (_textures.find(name) == _textures.end())
        {
            _textures[name] = te_shared_ptr_new<TextureData>(value, surface);
            return;
        }

        _textures[name]->TextureElem = value;
        _textures[name]->TextureSurfaceElem = surface;

        _markCoreDirty(MaterialDirtyFlags::ParamResource);
    }

    /** Assigns a buffer to the shader parameter with the specified name. */
    void Material::SetBuffer(const String& name, const SPtr<GpuBuffer>& value)
    {
        _buffers[name] = value;
        _markCoreDirty(MaterialDirtyFlags::ParamResource);
    }

    /** Assigns a sampler state to the shader parameter with the specified name. */
    void Material::SetSamplerState(const String& name, const SPtr<SamplerState>& value)
    {
        _samplerStates[name] = value;
        _markCoreDirty(MaterialDirtyFlags::ParamResource);
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
