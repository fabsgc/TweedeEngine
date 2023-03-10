#include "Material/TeShader.h"

#include "Material/TeTechnique.h"
#include "Material/TeShaderVariation.h"
#include "Resources/TeResourceManager.h"
#include "RenderAPI/TeSamplerState.h"
#include "RenderAPI/TeGpuParams.h"
#include "Image/TeTexture.h"

namespace te
{
    std::atomic<UINT32> Shader::NextShaderId;

    SHADER_DESC::SHADER_DESC()
        : QueueType(QueueSortType::None)
        , QueuePriority(0)
        , SeparablePasses(false), Flags(0)
    { }

    void SHADER_DESC::AddParameter(SHADER_DATA_PARAM_DESC paramDesc, UINT8* defaultValue)
    {
        if (paramDesc.Type == GPDT_STRUCT && paramDesc.ElementSize <= 0)
        {
            TE_DEBUG("You need to provide a non-zero element size for a struct parameter.");
            return;
        }

        const auto iterFind = DataParams.find(paramDesc.Name);
        if (iterFind != DataParams.end())
            return;

        if (defaultValue != nullptr)
        {
            paramDesc.DefaultValueIdx = (UINT32)DataDefaultValues.size();
            UINT32 defaultValueSize = Shader::GetDataParamSize(paramDesc.Type);

            DataDefaultValues.resize(paramDesc.DefaultValueIdx + defaultValueSize);
            memcpy(&DataDefaultValues[paramDesc.DefaultValueIdx], defaultValue, defaultValueSize);
        }
        else
        {
            paramDesc.DefaultValueIdx = (UINT32)-1;
        }

        DataParams[paramDesc.Name] = paramDesc;
    }

    void SHADER_DESC::AddParameter(SHADER_OBJECT_PARAM_DESC paramDesc)
    {
        UINT32 defaultValueIdx = (UINT32)-1;

        AddParameterInternal(std::move(paramDesc), defaultValueIdx);
    }

    void SHADER_DESC::AddParameter(SHADER_OBJECT_PARAM_DESC paramDesc, const SPtr<SamplerState>& defaultValue)
    {
        UINT32 defaultValueIdx = (UINT32)-1;
        if (Shader::IsSampler(paramDesc.Type) && defaultValue != nullptr)
        {
            defaultValueIdx = (UINT32)SamplerDefaultValues.size();
            SamplerDefaultValues.push_back(defaultValue);
        }

        AddParameterInternal(std::move(paramDesc), defaultValueIdx);
    }

    void SHADER_DESC::AddParameter(SHADER_OBJECT_PARAM_DESC paramDesc, const SPtr <Texture>& defaultValue)
    {
        UINT32 defaultValueIdx = (UINT32)-1;
        if (Shader::IsTexture(paramDesc.Type) && defaultValue != nullptr)
        {
            defaultValueIdx = (UINT32)TextureDefaultValues.size();
            TextureDefaultValues.push_back(defaultValue);
        }

        AddParameterInternal(std::move(paramDesc), defaultValueIdx);
    }

    void SHADER_DESC::AddParameterInternal(SHADER_OBJECT_PARAM_DESC paramDesc, UINT32 defaultValueIdx)
    {
        Map<String, SHADER_OBJECT_PARAM_DESC>* DEST_LOOKUP[] = { &TextureParams, &BufferParams, &SamplerParams };
        UINT32 destIdx = 0;
        if (Shader::IsBuffer(paramDesc.Type))
            destIdx = 1;
        else if (Shader::IsSampler(paramDesc.Type))
            destIdx = 2;

        Map<String, SHADER_OBJECT_PARAM_DESC>& paramsMap = *DEST_LOOKUP[destIdx];

        auto iterFind = paramsMap.find(paramDesc.Name);
        if (iterFind == paramsMap.end())
        {
            paramDesc.DefaultValueIdx = defaultValueIdx;
            paramsMap[paramDesc.Name] = paramDesc;
        }
        else
        {
            SHADER_OBJECT_PARAM_DESC& desc = iterFind->second;

            // If same name but different properties, we ignore this param
            if (desc.Type != paramDesc.Type)
                return;

            Vector<String>& gpuVariableNames = desc.GpuVariableNames;
            bool found = false;
            for (UINT32 i = 0; i < (UINT32)gpuVariableNames.size(); i++)
            {
                if (gpuVariableNames[i] == paramDesc.GpuVariableName)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
                gpuVariableNames.push_back(paramDesc.GpuVariableName);
        }
    }

    Shader::Shader()
        : Resource(TID_Shader)
        , _desc(SHADER_DESC())
        , _id(0)
    {
        UINT32 id = Shader::NextShaderId.fetch_add(1, std::memory_order_relaxed);
        assert(id < std::numeric_limits<UINT32>::max() && "Created too many shaders, reached maximum id.");
        _name = "shader";
    }

    Shader::Shader(UINT32 id)
        : Resource(TID_Shader)
        , _desc(SHADER_DESC())
        , _id(id)
    {
        _name = "shader";
    }

    Shader::Shader(const SHADER_DESC& desc, const String& name, UINT32 id)
        : Resource(TID_Shader)
        , _desc(desc)
        , _id(id)
    { 
        _name = name;
    }

    Shader::~Shader()
    { }

    void Shader::Initialize()
    { }

    Vector<SPtr<Technique>> Shader::GetCompatibleTechniques() const
    {
        Vector<SPtr<Technique>> output;
        for (auto& technique : _desc.Techniques)
        {
            if (technique->IsSupported())
                output.push_back(technique);
        }

        return output;
    }

    Vector<SPtr<Technique>> Shader::GetCompatibleTechniques(const ShaderVariation& variation, bool exact) const
    {
        Vector<SPtr<Technique>> output;
        for (auto& technique : _desc.Techniques)
        {
            if (technique->IsSupported() && technique->GetVariation().Matches(variation, exact))
                output.push_back(technique);
        }

        return output;
    }

    GpuParamType Shader::GetParamType(const String& name) const
    {
        auto findIterData = _desc.DataParams.find(name);
        if (findIterData != _desc.DataParams.end())
            return GPT_DATA;

        auto findIterTexture = _desc.TextureParams.find(name);
        if (findIterTexture != _desc.TextureParams.end())
            return GPT_TEXTURE;

        auto findIterBuffer = _desc.BufferParams.find(name);
        if (findIterBuffer != _desc.BufferParams.end())
            return GPT_BUFFER;

        auto findIterSampler = _desc.SamplerParams.find(name);
        if (findIterSampler != _desc.SamplerParams.end())
            return GPT_SAMPLER;

        TE_ASSERT_ERROR(false, "Cannot find the parameter with the name: " + name);
        return GPT_DATA;
    }

    const SHADER_DATA_PARAM_DESC& Shader::GetDataParamDesc(const String& name) const
    {
        auto findIterData = _desc.DataParams.find(name);
        if (findIterData != _desc.DataParams.end())
            return findIterData->second;

        TE_ASSERT_ERROR(false, "Cannot find the parameter with the name: " + name);
        static SHADER_DATA_PARAM_DESC dummy;
        return dummy;
    }

    const SHADER_OBJECT_PARAM_DESC& Shader::GetTextureParamDesc(const String& name) const
    {
        auto findIterObject = _desc.TextureParams.find(name);
        if (findIterObject != _desc.TextureParams.end())
            return findIterObject->second;

        TE_ASSERT_ERROR(false, "Cannot find the parameter with the name: " + name);
        static SHADER_OBJECT_PARAM_DESC dummy;
        return dummy;
    }

    const SHADER_OBJECT_PARAM_DESC& Shader::GetSamplerParamDesc(const String& name) const
    {
        auto findIterObject = _desc.SamplerParams.find(name);
        if (findIterObject != _desc.SamplerParams.end())
            return findIterObject->second;

        TE_ASSERT_ERROR(false, "Cannot find the parameter with the name: " + name);
        static SHADER_OBJECT_PARAM_DESC dummy;
        return dummy;
    }

    const SHADER_OBJECT_PARAM_DESC& Shader::GetBufferParamDesc(const String& name) const
    {
        auto findIterObject = _desc.BufferParams.find(name);
        if (findIterObject != _desc.BufferParams.end())
            return findIterObject->second;

        TE_ASSERT_ERROR(false, "Cannot find the parameter with the name: " + name);
        static SHADER_OBJECT_PARAM_DESC dummy;
        return dummy;
    }

    bool Shader::HasDataParam(const String& name) const
    {
        auto findIterData = _desc.DataParams.find(name);
        if (findIterData != _desc.DataParams.end())
            return true;

        return false;
    }

    bool Shader::HasTextureParam(const String& name) const
    {
        auto findIterObject = _desc.TextureParams.find(name);
        if (findIterObject != _desc.TextureParams.end())
            return true;

        return false;
    }

    bool Shader::HasSamplerParam(const String& name) const
    {
        auto findIterObject = _desc.SamplerParams.find(name);
        if (findIterObject != _desc.SamplerParams.end())
            return true;

        return false;
    }

    bool Shader::HasBufferParam(const String& name) const
    {
        auto findIterObject = _desc.BufferParams.find(name);
        if (findIterObject != _desc.BufferParams.end())
            return true;

        return false;
    }

    bool Shader::HasParamBlock(const String& name) const
    {
        auto findIterObject = _desc.ParamBlocks.find(name);
        if (findIterObject != _desc.ParamBlocks.end())
            return true;

        return false;
    }

    SPtr<Texture> Shader::GetDefaultTexture(UINT32 index) const
    {
        if (index < (UINT32)_desc.TextureDefaultValues.size())
            return _desc.TextureDefaultValues[index];

        return SPtr<Texture>();
    }

    SPtr<SamplerState> Shader::GetDefaultSampler(UINT32 index) const
    {
        if (index < (UINT32)_desc.SamplerDefaultValues.size())
            return _desc.SamplerDefaultValues[index];

        return SPtr<SamplerState>();
    }

    UINT8* Shader::GetDefaultValue(UINT32 index) const
    {
        if (index < (UINT32)_desc.DataDefaultValues.size())
            return (UINT8*)&_desc.DataDefaultValues[index];

        return nullptr;
    }

    void Shader::Compile(bool force)
    {
        for (auto& technique : _desc.Techniques)
        {
            technique->Compile(force);
        }
    }

    HShader Shader::Create(const String& name, const SHADER_DESC& desc)
    {
        UINT32 id = Shader::NextShaderId.fetch_add(1, std::memory_order_relaxed);
        assert(id < std::numeric_limits<UINT32>::max() && "Created too many shaders, reached maximum id.");

        SPtr<Shader> shader = te_core_ptr<Shader>(new (te_allocate<Shader>()) Shader(desc, name, id));
        shader->SetThisPtr(shader);
        shader->Initialize();

        return static_resource_cast<Shader>(gResourceManager()._createResourceHandle(shader));
    }

    SPtr<Shader> Shader::CreateEmpty()
    {
        UINT32 id = Shader::NextShaderId.fetch_add(1, std::memory_order_relaxed);
        assert(id < std::numeric_limits<UINT32>::max() && "Created too many shaders, reached maximum id.");

        SPtr<Shader> shader = te_core_ptr<Shader>(new (te_allocate<Shader>()) Shader());
        shader->SetThisPtr(shader);

        return shader;
    }

    SPtr<Shader> Shader::CreatePtr(const String& name, const SHADER_DESC& desc)
    {
        UINT32 id = Shader::NextShaderId.fetch_add(1, std::memory_order_relaxed);
        assert(id < std::numeric_limits<UINT32>::max() && "Created too many shaders, reached maximum id.");

        SPtr<Shader> shader = te_core_ptr<Shader>(new (te_allocate<Shader>()) Shader(desc, name, id));
        shader->SetThisPtr(shader);
        shader->Initialize();

        return shader;
    }

    bool Shader::IsSampler(GpuParamObjectType type)
    {
        switch (type)
        {
        case GPOT_SAMPLER1D:
        case GPOT_SAMPLER2D:
        case GPOT_SAMPLER3D:
        case GPOT_SAMPLERCUBE:
        case GPOT_SAMPLER2DMS:
            return true;
        default:
            return false;
        }
    }

    bool Shader::IsTexture(GpuParamObjectType type)
    {
        switch (type)
        {
        case GPOT_TEXTURE1D:
        case GPOT_TEXTURE2D:
        case GPOT_TEXTURE3D:
        case GPOT_TEXTURECUBE:
        case GPOT_TEXTURE2DMS:
        case GPOT_TEXTURE1DARRAY:
        case GPOT_TEXTURE2DARRAY:
        case GPOT_TEXTURE2DMSARRAY:
        case GPOT_TEXTURECUBEARRAY:
            return true;
        default:
            return false;
        }
    }

    bool Shader::IsLoadStoreTexture(GpuParamObjectType type)
    {
        switch (type)
        {
        case GPOT_RWTEXTURE1D:
        case GPOT_RWTEXTURE2D:
        case GPOT_RWTEXTURE3D:
        case GPOT_RWTEXTURE2DMS:
        case GPOT_RWTEXTURE1DARRAY:
        case GPOT_RWTEXTURE2DARRAY:
        case GPOT_RWTEXTURE2DMSARRAY:
            return true;
        default:
            return false;
        }
    }

    bool Shader::IsBuffer(GpuParamObjectType type)
    {
        switch (type)
        {
        case GPOT_BYTE_BUFFER:
        case GPOT_STRUCTURED_BUFFER:
        case GPOT_RWBYTE_BUFFER:
        case GPOT_RWAPPEND_BUFFER:
        case GPOT_RWCONSUME_BUFFER:
        case GPOT_RWSTRUCTURED_BUFFER:
        case GPOT_RWSTRUCTURED_BUFFER_WITH_COUNTER:
        case GPOT_RWTYPED_BUFFER:
            return true;
        default:
            return false;
        }
    }

    UINT32 Shader::GetDataParamSize(GpuParamDataType type)
    {
        static const GpuDataParamInfos PARAM_SIZES;

        UINT32 idx = (UINT32)type;
        if (idx < sizeof(GpuParams::PARAM_SIZES.lookup))
            return GpuParams::PARAM_SIZES.lookup[idx].size;

        return 0;
    }
}
