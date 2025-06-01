#include "TeMaterial.h"

#include "TeShader.h"
#include "TePass.h"
#include "TeTechnique.h"
#include "TeShaderVariation.h"
#include "Image/TeTexture.h"
#include "RenderAPI/TeSamplerState.h"
#include "Resources/TeResourceHandle.h"
#include "Resources/TeResourceManager.h"
#include "Resources/TeBuiltinResources.h"
#include "ThirdParty/Slugify/slugify.hpp"
#include "Serialization/TeUtility.h"

namespace te
{
    std::atomic<UINT32> Material::NextMaterialId;

    Material::Material()
        : Resource(CoreType::TID_Material)
        , _id(0)
    { 
        UINT32 id = Material::NextMaterialId.fetch_add(1, std::memory_order_relaxed);
        assert(id < std::numeric_limits<UINT32>::max() && "Created too many materials, reached maximum id.");

        _id = id;
    }

    Material::Material(UINT32 id, const ShaderVariation& variation)
        : Resource(CoreType::TID_Material)
        , _id(id)
        , _variation(variation)
    { }

    Material::Material(const HShader& shader, const ShaderVariation& variation, UINT32 id)
    : Material(id, variation)
    {
        if(shader.IsLoaded())
            SetShader(shader);
    }

    Material::~Material()
    {
        for (auto& param : _params)
        {
            if(param.second.Param)
                te_deallocate(param.second.Param); //I's sure that types here are primitive or very simple type sush as Vector3
        }
    }

    void Material::Initialize()
    {
        Resource::Initialize();

        SetSamplerState("AnisotropicSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Anisotropic));
        SetSamplerState("BilinearSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Bilinear));
        SetSamplerState("NoFilterSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::NoFilter));

        InitializeTechniques();
    }

    void Material::InitializeTechniques()
    {
        _techniques.clear();

        if (_shader.IsLoaded())
        {
            _shader->GetCompatibleTechniques(_techniques);

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
                outputParams[idx]->SetTexture(texture.first, texture.second->TextureElem.GetInternalPtr(), texture.second->TextureSurfaceElem);

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

    void Material::SetShader(const HShader& shader)
    {
        _shader = shader;

        if (shader.IsLoaded())
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
        const auto it = _techniques.find(idx);

        if (it != _techniques.end())
            return it->second;

        TE_ASSERT_ERROR(false, "Can't find technique with idx : " + ToString(idx));

        return _techniques.begin()->second;
    }

    UINT32 Material::FindTechnique(const FIND_TECHNIQUE_DESC& desc, bool createTechnique) const
    {
        UINT32 bestTechniqueIdx = (UINT32)-1;
        UINT32 bestTechniqueScore = std::numeric_limits<UINT32>::max();

        for (const auto& technique : _techniques)
        {
            // Make sure tags match
            bool foundMatch = true;
            for (UINT32 j = 0; j < desc.NumTags; j++)
            {
                if (!technique.second->HasTag(desc.Tags[j]))
                {
                    foundMatch = false;
                    break;
                }
            }

            if (!foundMatch)
                continue;

            const ShaderVariation& curVariation = technique.second->GetVariation();
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

                const auto& searchVarParams = desc.Variation.GetParams();
                const auto findSearch = searchVarParams.find(param.first);
                if (findSearch != searchVarParams.end())
                    matchesSearch = findSearch->second.I == param.second.I ? Matching : NotMatching;

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
                    if (desc.Override)
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
                        if (desc.Override)
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

            const auto& searchVarParams = desc.Variation.GetParams();
            if (numMatchedSearchParams != (UINT32)searchVarParams.size())
                continue;

            if (numMatchedInternalParams != (UINT32)internalVarParams.size())
                continue;

            if (currentScore < bestTechniqueScore)
            {
                bestTechniqueIdx = technique.second->GetId();
                bestTechniqueScore = currentScore;
            }
        }

        if (bestTechniqueIdx == (UINT32)-1 && createTechnique && _shader.IsLoaded())
        {
            SPtr<Technique> newTechnique = _shader->CreateTechnique(desc.Variation, desc.Tags);
            if (newTechnique)
            {
                bestTechniqueIdx = newTechnique->GetId();
                _techniques[bestTechniqueIdx] = newTechnique;
            }
        }

        return bestTechniqueIdx;
    }

    UINT32 Material::GetDefaultTechnique(bool createTechnique) const
    {
        UINT32 bestTechniqueIdx = (UINT32)-1;
        UINT32 bestTechniqueScore = std::numeric_limits<UINT32>::max();

        for (const auto& technique : _techniques)
        {
            if (technique.second->HasTags())
                continue;

            const ShaderVariation& curVariation = technique.second->GetVariation();
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
                bestTechniqueIdx = technique.second->GetId();
                bestTechniqueScore = currentScore;
            }
        }

        if (bestTechniqueIdx == (UINT32)-1 && createTechnique && _shader.IsLoaded())
        {
            SPtr<Technique> newTechnique = _shader->CreateTechnique(ShaderVariation(), {});
            if (newTechnique)
            {
                bestTechniqueIdx = newTechnique->GetId();
                _techniques[bestTechniqueIdx] = newTechnique;
            }
        }

        return bestTechniqueIdx;
    }

    UINT32 Material::GetNumPasses(UINT32 techniqueIdx) const
    {
        if (!_shader.IsLoaded())
            return 0;

        const auto& it = _techniques.find(techniqueIdx);
        if (it == _techniques.end())
            return 0;

        return it->second->GetNumPasses();
    }

    const SPtr<Pass> Material::GetPass(UINT32 passIdx, UINT32 techniqueIdx) const
    {
        if (!_shader.IsLoaded())
            return nullptr;

        const auto& it = _techniques.find(techniqueIdx);
        if (it == _techniques.end())
            return nullptr;

        if (passIdx >= it->second->GetNumPasses())
            return nullptr;

        return it->second->GetPass(passIdx);
    }

    /** Assigns a texture to the shader parameter with the specified name. */
    void Material::SetTexture(const String& name, HTexture value, const TextureSurface& surface)
    {
#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
        TE_ASSERT_ERROR(value.IsLoaded(), "Texture should not be null");
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

    HTexture Material::GetTexture(const String& name)
    {
        auto it = _textures.find(name);
        if (it == _textures.end())
            return HTexture();

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

    void Material::SetLoadStoreTexture(const String& name, HTexture value, const TextureSurface& surface)
    {
#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
        TE_ASSERT_ERROR(value.IsLoaded(), "Load store texture should not be null");
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
#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
        TE_ASSERT_ERROR(value != nullptr, "Buffer should not be null");
#endif

        _buffers[name] = value;
        _markCoreDirty(MaterialDirtyFlags::ParamResource);
    }

    /** Assigns a sampler state to the shader parameter with the specified name. */
    void Material::SetSamplerState(const String& name, const SPtr<SamplerState>& value)
    {
#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
        TE_ASSERT_ERROR(value != nullptr, "Sampler state should not be null");
#endif
        _samplerStates[name] = value;
        _markCoreDirty(MaterialDirtyFlags::ParamResource);
    }

    const SPtr<SamplerState>& Material::GetSamplerState(const String& name)
    {
        return _samplerStates[name];
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

        SPtr<Material> materialPtr = te_core_ptr<Material>(new (te_allocate<Material>()) Material(shader, ShaderVariation::EMPTY, id));
        materialPtr->SetThisPtr(materialPtr);
        materialPtr->Initialize();

        return static_resource_cast<Material>(gResourceManager()._createResourceHandle(materialPtr));
    }

    HMaterial Material::Create(const HShader& shader, const ShaderVariation& variation)
    {
        UINT32 id = Material::NextMaterialId.fetch_add(1, std::memory_order_relaxed);
        assert(id < std::numeric_limits<UINT32>::max() && "Created too many materials, reached maximum id.");

        SPtr<Material> materialPtr = te_core_ptr<Material>(new (te_allocate<Material>()) Material(shader, variation, id));
        materialPtr->SetThisPtr(materialPtr);
        materialPtr->Initialize();

        return static_resource_cast<Material>(gResourceManager()._createResourceHandle(materialPtr));
    }

    SPtr<Material> Material::CreateEmpty()
    {
        UINT32 id = Material::NextMaterialId.fetch_add(1, std::memory_order_relaxed);
        assert(id < std::numeric_limits<UINT32>::max() && "Created too many materials, reached maximum id.");

        SPtr<Material> newMat = te_core_ptr<Material>(new (te_allocate<Material>()) Material());
        newMat->SetThisPtr(newMat);
        newMat->_id = id;

        return newMat;
    }

    void Material::_markCoreDirty(MaterialDirtyFlags flags)
    {
        MarkCoreDirty((UINT32)flags);
    }

    void Material::Serialize(StreamWriter* serializer) const
    {
        Resource::Serialize(serializer);

        nlohmann::json document;

        document["shader"] = (_shader.IsLoaded() && gBuiltinResources().IsBuiltInResource(_shader->GetUUID())) ? serialization::GetResourceName(_shader.Get()) : "";
        _properties.ExportJson(document["properties"]);

        String dump = document.dump();
        serializer->WriteString(dump);

        // TODO Serialization
    }

    bool Material::Deserialize(StreamReader* deserializer, Material* object)
    {
        if (!object)
            return false;

        Resource::Deserialize(deserializer, object);

        String dump;
        deserializer->ReadString(dump);
        nlohmann::json document = nlohmann::json::parse(dump);

        object->_properties = MaterialProperties::ImportJson(document["properties"]);

        object->Initialize();
        object->SetShader(gBuiltinResources().GetBuiltinShader(BuiltinShader::Opaque));

        // TODO Serialization

        return true;
    }

    void MaterialProperties::ExportJson(nlohmann::json& document) const
    {
        document = {
            { "metallic", Metallic },
            { "roughness", Roughness },
            { "reflectance", Reflectance },
            { "occlusion", Occlusion },
            { "sheenRoughness", SheenRoughness },
            { "clearCoat", ClearCoat },
            { "clearCoatRoughness", ClearCoatRoughness },
            { "subsurfacePower", SubsurfacePower },
            { "anisotropy", Anisotropy },
            { "alphaThreshold", AlphaThreshold },
            { "parallaxScale", ParallaxScale },
            { "parallaxSamples", ParallaxSamples },
            { "microThickness", MicroThickness },
            { "thickness", Thickness },
            { "transmission", Transmission },
            { "atDistance", AtDistance },
            { "refractType", RefractType },
            { "useBaseColorMap", UseBaseColorMap },
            { "useMetallicMap", UseMetallicMap },
            { "useRoughnessMap", UseRoughnessMap },
            { "useMetallicRoughnessMap", UseMetallicRoughnessMap },
            { "useReflectanceMap", UseReflectanceMap },
            { "useOcclusionMap", UseOcclusionMap },
            { "useEmissiveMap", UseEmissiveMap },
            { "useSheenColorMap", UseSheenColorMap },
            { "useSheenRoughnessMap", UseSheenRoughnessMap },
            { "useClearCoatMap", UseClearCoatMap },
            { "useClearCoatRoughnessMap", UseClearCoatRoughnessMap },
            { "useClearCoatNormalMap", UseClearCoatNormalMap },
            { "useNormalMap", UseNormalMap },
            { "useParallaxMap", UseParallaxMap },
            { "useTransmissionMap", UseTransmissionMap },
            { "useOpacityMap", UseOpacityMap },
            { "useAnisotropyDirectionMap", UseAnisotropyDirectionMap },
            { "doIndirectLighting", DoIndirectLighting },
            { "doDirectLighting", DoDirectLighting }
        };

        BaseColor.ExportJson(document["baseColor"]);
        Emissive.ExportJson(document["emissive"]);
        SheenColor.ExportJson(document["sheenColor"]);
        SubsurfaceColor.ExportJson(document["subsurfaceColor"]);
        AnisotropyDirection.ExportJson(document["anisotropyDirection"]);
        TextureRepeat.ExportJson(document["textureRepeat"]);
        TextureOffset.ExportJson(document["textureOffset"]);
        TransmittanceColor.ExportJson(document["transmittanceColor"]);
        Absorption.ExportJson(document["absorption"]);
    }

    MaterialProperties MaterialProperties::ImportJson(const nlohmann::json& document)
    {
        MaterialProperties properties;

        properties.BaseColor = Color::ImportJson(document["baseColor"]);
        properties.Metallic = document["metallic"].get<float>();
        properties.Roughness = document["roughness"].get<float>();
        properties.Reflectance = document["reflectance"].get<float>();
        properties.Occlusion = document["occlusion"].get<float>();
        properties.Emissive = Color::ImportJson(document["emissive"]);
        properties.SheenColor = Color::ImportJson(document["sheenColor"]);
        properties.SheenRoughness = document["sheenRoughness"].get<float>();
        properties.ClearCoat = document["clearCoat"].get<float>();
        properties.ClearCoatRoughness = document["clearCoatRoughness"].get<float>();
        properties.SubsurfaceColor = Color::ImportJson(document["subsurfaceColor"]);
        properties.SubsurfacePower = document["subsurfacePower"].get<float>();
        properties.Anisotropy = document["anisotropy"].get<float>();
        properties.AnisotropyDirection = Vector3::ImportJson(document["anisotropyDirection"]);
        properties.AlphaThreshold = document["alphaThreshold"].get<float>();
        properties.TextureRepeat = Vector2::ImportJson(document["textureRepeat"]);
        properties.TextureOffset = Vector2::ImportJson(document["textureOffset"]);
        properties.ParallaxScale = document["parallaxScale"].get<float>();
        properties.ParallaxSamples = document["parallaxSamples"].get<uint32_t>();
        properties.MicroThickness = document["microThickness"].get<float>();
        properties.Thickness = document["thickness"].get<float>();
        properties.Transmission = document["transmission"].get<float>();
        properties.TransmittanceColor = Color::ImportJson(document["transmittanceColor"]);
        properties.AtDistance = document["atDistance"].get<float>();
        properties.Absorption = Vector3::ImportJson(document["absorption"]);
        properties.RefractType = static_cast<RefractionType>(document["refractType"].get<uint32_t>());
        properties.UseBaseColorMap = document["useBaseColorMap"].get<bool>();
        properties.UseMetallicMap = document["useMetallicMap"].get<bool>();
        properties.UseRoughnessMap = document["useRoughnessMap"].get<bool>();
        properties.UseMetallicRoughnessMap = document["useMetallicRoughnessMap"].get<bool>();
        properties.UseReflectanceMap = document["useReflectanceMap"].get<bool>();
        properties.UseOcclusionMap = document["useOcclusionMap"].get<bool>();
        properties.UseEmissiveMap = document["useEmissiveMap"].get<bool>();
        properties.UseSheenColorMap = document["useSheenColorMap"].get<bool>();
        properties.UseSheenRoughnessMap = document["useSheenRoughnessMap"].get<bool>();
        properties.UseClearCoatMap = document["useClearCoatMap"].get<bool>();
        properties.UseClearCoatRoughnessMap = document["useClearCoatRoughnessMap"].get<bool>();
        properties.UseClearCoatNormalMap = document["useClearCoatNormalMap"].get<bool>();
        properties.UseNormalMap = document["useNormalMap"].get<bool>();
        properties.UseParallaxMap = document["useParallaxMap"].get<bool>();
        properties.UseTransmissionMap = document["useTransmissionMap"].get<bool>();
        properties.UseOpacityMap = document["useOpacityMap"].get<bool>();
        properties.UseAnisotropyDirectionMap = document["useAnisotropyDirectionMap"].get<bool>();
        properties.DoIndirectLighting = document["doIndirectLighting"].get<bool>();
        properties.DoDirectLighting = document["doDirectLighting"].get<bool>();

        return properties;
    }

    void MaterialTextures::ExportJson(nlohmann::json& document) const
    {
        document = {
            { "baseColorMap", BaseColorMap },
            { "metallicMap", MetallicMap },
            { "roughnessMap", RoughnessMap },
            { "metallicRoughnessMap", MetallicRoughnessMap },
            { "reflectanceMap", ReflectanceMap },
            { "occlusionMap", OcclusionMap },
            { "emissiveMap", EmissiveMap },
            { "sheenColorMap", SheenColorMap },
            { "sheenRoughnessMap", SheenRoughnessMap },
            { "clearCoatMap", ClearCoatMap },
            { "clearCoatRoughnessMap", ClearCoatRoughnessMap },
            { "clearCoatNormalMap", ClearCoatNormalMap },
            { "normalMap", NormalMap },
            { "parallaxMap", ParallaxMap },
            { "transmissionMap", TransmissionMap },
            { "opacityMap", OpacityMap },
            { "anisotropyDirectionMap", AnisotropyDirectionMap },
        };
    }

    MaterialTextures MaterialTextures::ImportJson(const nlohmann::json& document)
    {
        MaterialTextures textures;

        textures.BaseColorMap = document["baseColorMap"].get<String>();
        textures.MetallicMap = document["metallicMap"].get<String>();
        textures.RoughnessMap = document["roughnessMap"].get<String>();
        textures.MetallicRoughnessMap = document["metallicRoughnessMap"].get<String>();
        textures.ReflectanceMap = document["reflectanceMap"].get<String>();
        textures.OcclusionMap = document["occlusionMap"].get<String>();
        textures.EmissiveMap = document["emissiveMap"].get<String>();
        textures.SheenColorMap = document["sheenColorMap"].get<String>();
        textures.SheenRoughnessMap = document["sheenRoughnessMap"].get<String>();
        textures.ClearCoatMap = document["clearCoatMap"].get<String>();
        textures.ClearCoatRoughnessMap = document["clearCoatRoughnessMap"].get<String>();
        textures.ClearCoatNormalMap = document["clearCoatNormalMap"].get<String>();
        textures.NormalMap = document["normalMap"].get<String>();
        textures.ParallaxMap = document["parallaxMap"].get<String>();
        textures.TransmissionMap = document["transmissionMap"].get<String>();
        textures.OpacityMap = document["opacityMap"].get<String>();
        textures.AnisotropyDirectionMap = document["anisotropyDirectionMap"].get<String>();

        return textures;
    }

    void Material::OnResourceModified(const HResource& resource)
    {
        if (resource.GetUUID() == GetUUID())
            return;

        if (resource.IsLoaded() && resource->GetCoreType() == CoreType::TID_Texture)
        {
            for (const auto& texture : _textures)
            {
                if (texture.second->TextureElem.GetUUID() == resource.GetUUID())
                {
                    _markCoreDirty(MaterialDirtyFlags::ParamResource);
                    gResourceManager().OnResourceModified(gResourceManager().Get(GetUUID()));
                    break;
                }
            }

            for (const auto& texture : _loadStoreTextures)
            {
                if (texture.second->TextureElem.GetUUID() == resource.GetUUID())
                {
                    _markCoreDirty(MaterialDirtyFlags::ParamResource);
                    gResourceManager().OnResourceModified(gResourceManager().Get(GetUUID()));
                    break;
                }
            }
        }

        if (resource.IsLoaded() && resource->GetCoreType() == CoreType::TID_Shader)
        {
            if (_shader.GetUUID() == resource.GetUUID())
            {
                InitializeTechniques();
                gResourceManager().OnResourceModified(gResourceManager().Get(GetUUID()));
            }
        }
    }

    void Material::OnResourceDestroyed(const UUID& uuid, CoreType type)
    {
        if (uuid == GetUUID())
            return;

        if (type == CoreType::TID_Texture)
        {
            for (auto& texture : _textures)
            {
                if (texture.second->TextureElem.IsLoaded() && texture.second->TextureElem->GetUUID() == uuid)
                {
                    _textures.erase(texture.first);
                    _markCoreDirty(MaterialDirtyFlags::ParamResource);
                    gResourceManager().OnResourceModified(gResourceManager().Get(GetUUID()));
                    break;
                }
            }

            for (auto& texture : _loadStoreTextures)
            {
                if (texture.second->TextureElem.IsLoaded() && texture.second->TextureElem->GetUUID() == uuid)
                {
                    _loadStoreTextures.erase(texture.first);
                    _markCoreDirty(MaterialDirtyFlags::ParamResource);
                    gResourceManager().OnResourceModified(gResourceManager().Get(GetUUID()));
                    break;
                }
            }
        }

        if (type == CoreType::TID_Shader)
        {
            if (_shader.IsLoaded() && _shader->GetUUID() == uuid)
            {
                _shader = nullptr;
                InitializeTechniques();
                gResourceManager().OnResourceModified(gResourceManager().Get(GetUUID()));
            }
        }
    }
}
