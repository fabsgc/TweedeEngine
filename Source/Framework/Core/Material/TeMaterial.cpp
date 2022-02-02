#include "TeMaterial.h"
#include "TeShader.h"
#include "TePass.h"
#include "TeTechnique.h"
#include "TeShaderVariation.h"
#include "Image/TeTexture.h"
#include "RenderAPI/TeSamplerState.h"
#include "Resources/TeResourceHandle.h"
#include "Resources/TeResourceManager.h"

namespace te
{
    std::atomic<UINT32> Material::NextMaterialId;

    Material::Material()
        : Resource(TID_Material)
        , _id(0)
    { 
        UINT32 id = Material::NextMaterialId.fetch_add(1, std::memory_order_relaxed);
        assert(id < std::numeric_limits<UINT32>::max() && "Created too many materials, reached maximum id.");
    }

    Material::Material(UINT32 id)
        : Resource(TID_Material)
        , _id(id)
    { }

    Material::~Material()
    {
        for (auto& param : _params)
        {
            if(param.second.Param)
                te_deallocate(param.second.Param); //I's sure that types here are primitive or very simple type sush as Vector3
        }
    }

    Material::Material(const HShader& shader, UINT32 id)
        : Material(id)
    {
        SetShader(shader.GetInternalPtr());
    }

    Material::Material(const SPtr<Shader>& shader, UINT32 id)
        : Material(id)
    {
        SetShader(shader);
    }

    Material::Material(const HShader& shader, const Vector<SPtr<Technique>>& techniques, UINT32 id)
        : Resource(TID_Material)
        , _id(id)
        , _shader(shader.GetInternalPtr())
        , _techniques(techniques)
    { }

    Material::Material(const SPtr<Shader>& shader, const Vector<SPtr<Technique>>& techniques, UINT32 id)
        : Resource(TID_Material)
        , _id(id)
        , _shader(shader)
        , _techniques(techniques)
    { }

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
            {
                if(param.second.ProgramType == GpuProgramType::GPT_COUNT)
                    outputParams[idx]->SetParam(param.first, param.second.Param, (UINT32)param.second.Size);
                else
                    outputParams[idx]->SetParam(param.second.ProgramType, param.first, param.second.Param, (UINT32)param.second.Size);
            }  
        }
    }

    void Material::SetGpuParam(SPtr<GpuParams> outparams)
    {
        for (auto& param : _params)
        {
            if (param.second.ProgramType == GpuProgramType::GPT_COUNT)
                outparams->SetParam(param.first, param.second.Param, (UINT32)param.second.Size);
            else
                outparams->SetParam(param.second.ProgramType, param.first, param.second.Param, (UINT32)param.second.Size);
        }
    }

    void Material::SetShader(const SPtr<Shader>& shader)
    {
        _shader = shader;
        InitializeTechniques();
    }

    void Material::SetVariation(const ShaderVariation& variation)
    {
        _variation = variation;
    }

    UINT32 Material::GetNumTechniques() const
    {
        return (UINT32)_techniques.size();
    }

    const SPtr<Technique>& Material::GetTechnique(UINT32 idx) const 
    {
        return _techniques[idx];
    }

    UINT32 Material::FindTechnique(const FIND_TECHNIQUE_DESC& desc) const
    {
        UINT32 bestTechniqueIdx = (UINT32)-1;
        UINT32 bestTechniqueScore = std::numeric_limits<UINT32>::max();

        for (UINT32 i = 0; i < (UINT32)_techniques.size(); i++)
        {
            // Make sure tags match
            bool foundMatch = true;
            for (UINT32 j = 0; j < desc.NumTags; j++)
            {
                if (!_techniques[i]->HasTag(desc.Tags[j]))
                {
                    foundMatch = false;
                    break;
                }
            }

            if (!foundMatch)
                continue;

            const ShaderVariation& curVariation = _techniques[i]->GetVariation();
            const auto& curVarParams = curVariation.GetParams();
            const auto& internalVarParams = _variation.GetParams();

            UINT32 numMatchedSearchParams = 0;
            UINT32 numMatchedInternalParams = 0;
            UINT32 currentScore = 0;
            for (auto& param : curVarParams)
            {
                enum SearchResult
                {
                    NoParam,
                    NotMatching,
                    Matching
                };

                SearchResult matchesSearch = NoParam;
                if (desc.Variation)
                {
                    const auto& searchVarParams = desc.Variation->GetParams();
                    const auto findSearch = searchVarParams.find(param.first);
                    if (findSearch != searchVarParams.end())
                        matchesSearch = findSearch->second.I == param.second.I ? Matching : NotMatching;
                }

                SearchResult matchesInternal = NoParam;
                const auto findInternal = internalVarParams.find(param.first);
                if (findInternal != internalVarParams.end())
                    matchesInternal = findInternal->second.I == param.second.I ? Matching : NotMatching;

                switch (matchesSearch)
                {
                default:
                case NoParam:
                    switch (matchesInternal)
                    {
                    default:
                    case NoParam:
                        // When it comes to parameters not part of the search, prefer those with 0 default value
                        currentScore += param.second.Ui;
                        break;
                    case NotMatching:
                        foundMatch = false;
                        break;
                    case Matching:
                        numMatchedInternalParams++;
                        break;
                    }
                    break;
                case NotMatching:
                    if (desc.override)
                    {
                        foundMatch = false;
                        break;
                    }

                    switch (matchesInternal)
                    {
                    default:
                    case NoParam:
                        foundMatch = false;
                        break;
                    case NotMatching:
                        foundMatch = false;
                        break;
                    case Matching:
                        numMatchedSearchParams++;
                        numMatchedInternalParams++;
                        break;
                    }
                    break;
                case Matching:
                    switch (matchesInternal)
                    {
                    default:
                    case NoParam:
                        numMatchedSearchParams++;
                        break;
                    case NotMatching:
                        if (desc.override)
                        {
                            numMatchedSearchParams++;
                            numMatchedInternalParams++;
                        }
                        else
                            foundMatch = false;
                        break;
                    case Matching:
                        numMatchedSearchParams++;
                        numMatchedInternalParams++;
                        break;
                    }
                    break;
                }

                if (!foundMatch)
                    break;
            }

            if (!foundMatch)
                continue;

            if (desc.Variation)
            {
                const auto& searchVarParams = desc.Variation->GetParams();
                if (numMatchedSearchParams != (UINT32)searchVarParams.size())
                    continue;
            }

            if (numMatchedInternalParams != (UINT32)internalVarParams.size())
                continue;

            if (currentScore < bestTechniqueScore)
            {
                bestTechniqueIdx = i;
                bestTechniqueScore = currentScore;
            }
        }

        return bestTechniqueIdx;
    }

    UINT32 Material::GetDefaultTechnique() const
    {
        UINT32 bestTechniqueIdx = 0;
        UINT32 bestTechniqueScore = std::numeric_limits<UINT32>::max();

        for (UINT32 i = 0; i < (UINT32)_techniques.size(); i++)
        {
            if (_techniques[i]->HasTags())
                continue;

            const ShaderVariation& curVariation = _techniques[i]->GetVariation();
            const auto& curVarParams = curVariation.GetParams();
            const auto& internalVarParams = _variation.GetParams();

            bool foundMatch = true;
            UINT32 numMatchedParams = 0;
            UINT32 currentScore = 0;
            for (auto& param : curVarParams)
            {
                enum SearchResult
                {
                    NoParam,
                    NotMatching,
                    Matching
                };

                SearchResult matches = NoParam;
                const auto findInternal = internalVarParams.find(param.first);
                if (findInternal != internalVarParams.end())
                    matches = findInternal->second.I == param.second.I ? Matching : NotMatching;

                switch (matches)
                {
                default:
                case NoParam:
                    // When it comes to parameters not part of the search, prefer those with 0 default value
                    currentScore += param.second.Ui;
                    break;
                case NotMatching:
                    foundMatch = false;
                    break;
                case Matching:
                    numMatchedParams++;
                    break;
                }

                if (!foundMatch)
                    break;
            }

            if (!foundMatch)
                continue;

            if (numMatchedParams != (UINT32)internalVarParams.size())
                continue;

            if (currentScore < bestTechniqueScore)
            {
                bestTechniqueIdx = i;
                bestTechniqueScore = currentScore;
            }
        }

        return bestTechniqueIdx;
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
#if TE_DEBUG_MODE
        TE_ASSERT_ERROR(value != nullptr, "Texture should not be null");
#endif
        auto it = _textures.find(name);
        if (it == _textures.end())
        {
            _textures[name] = te_shared_ptr_new<TextureData>(value, surface);
            return;
        }

        it->second->TextureElem = value;
        it->second->TextureSurfaceElem = surface;

        _markCoreDirty(MaterialDirtyFlags::ParamResource);
    }

    SPtr<Texture> Material::GetTexture(const String& name)
    {
        auto it = _textures.find(name);
        if (it == _textures.end())
            return nullptr;

        return it->second->TextureElem;
    }

    void Material::RemoveTexture(const String& name)
    {
        auto it = _textures.find(name);
        if (it != _textures.end())
        {
            _textures.erase(it);
        }
    }

    void Material::SetLoadStoreTexture(const String& name, const SPtr<Texture>& value, const TextureSurface& surface)
    {
#if TE_DEBUG_MODE
        TE_ASSERT_ERROR(value != nullptr, "Load store texture should not be null");
#endif
        auto it = _loadStoreTextures.find(name);
        if (it == _loadStoreTextures.end())
        {
            it->second = te_shared_ptr_new<TextureData>(value, surface);
            return;
        }

        it->second->TextureElem = value;
        it->second->TextureSurfaceElem = surface;

        _markCoreDirty(MaterialDirtyFlags::ParamResource);
    }

    /** Assigns a buffer to the shader parameter with the specified name. */
    void Material::SetBuffer(const String& name, const SPtr<GpuBuffer>& value)
    {
#if TE_DEBUG_MODE
        TE_ASSERT_ERROR(value != nullptr, "Buffer should not be null");
#endif

        _buffers[name] = value;
        _markCoreDirty(MaterialDirtyFlags::ParamResource);
    }

    /** Assigns a sampler state to the shader parameter with the specified name. */
    void Material::SetSamplerState(const String& name, const SPtr<SamplerState>& value)
    {
#if TE_DEBUG_MODE
        TE_ASSERT_ERROR(value != nullptr, "Sampler state should not be null");
#endif
        _samplerStates[name] = value;
        _markCoreDirty(MaterialDirtyFlags::ParamResource);
    }

    const SPtr<SamplerState>& Material::GetSamplerState(const String& name)
    {
        return _samplerStates[name];
    }

    void Material::SetTexture(const String& name, const HTexture& value, const TextureSurface& surface)
    {
        SetTexture(name, value.GetInternalPtr(), surface);
    }

    void Material::SetLoadStoreTexture(const String& name, const HTexture& value, const TextureSurface& surface)
    {
        SetLoadStoreTexture(name, value.GetInternalPtr(), surface);
    }

    HMaterial Material::Create()
    {
        const SPtr<Material> materialPtr = CreateEmpty();
        materialPtr->Initialize();

        return static_resource_cast<Material>(gResourceManager()._createResourceHandle(materialPtr));
    }

    HMaterial Material::Create(const HShader& shader)
    {
        UINT32 id = Material::NextMaterialId.fetch_add(1, std::memory_order_relaxed);
        assert(id < std::numeric_limits<UINT32>::max() && "Created too many materials, reached maximum id.");

        SPtr<Material> materialPtr = te_core_ptr<Material>(new (te_allocate<Material>()) Material(shader, id));
        materialPtr->SetThisPtr(materialPtr);
        materialPtr->Initialize();

        return static_resource_cast<Material>(gResourceManager()._createResourceHandle(materialPtr));
    }

    HMaterial Material::Create(const SPtr<Shader>& shader)
    {
        UINT32 id = Material::NextMaterialId.fetch_add(1, std::memory_order_relaxed);
        assert(id < std::numeric_limits<UINT32>::max() && "Created too many materials, reached maximum id.");

        SPtr<Material> materialPtr = te_core_ptr<Material>(new (te_allocate<Material>()) Material(shader, id));
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
