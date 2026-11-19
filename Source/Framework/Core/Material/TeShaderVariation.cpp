#include "TeShaderVariation.h"

namespace te
{
    void ShaderDefines::Set(const String& name, float value)
    {
        _defines[name] = ToString(value);
    }

    void ShaderDefines::Set(const String& name, INT32 value)
    {
        _defines[name] = ToString(value);
    }

    void ShaderDefines::Set(const String& name, UINT32 value)
    {
        _defines[name] = ToString(value);
    }

    void ShaderDefines::Set(const String& name, const String& value)
    {
        _defines[name] = value;
    }

    const ShaderVariation ShaderVariation::EMPTY;

    ShaderVariation::ShaderVariation(const Vector<Param>& params)
    {
        for (auto& entry : params)
            _params[entry.Name] = entry;
    }

    INT32 ShaderVariation::GetInt(const String& name)
    {
        auto iterFind = _params.find(name);
        if (iterFind == _params.end())
            return 0;
        else
            return iterFind->second.I;
    }

    UINT32 ShaderVariation::GetUInt(const String& name)
    {
        auto iterFind = _params.find(name);
        if (iterFind == _params.end())
            return 0;
        else
            return iterFind->second.Ui;
    }

    float ShaderVariation::GetFloat(const String& name)
    {
        auto iterFind = _params.find(name);
        if (iterFind == _params.end())
            return 0.0f;
        else
            return iterFind->second.F;
    }

    bool ShaderVariation::GetBool(const String& name)
    {
        auto iterFind = _params.find(name);
        if (iterFind == _params.end())
            return false;
        else
            return iterFind->second.I > 0 ? true : false;
    }

    void ShaderVariation::SetInt(const String& name, INT32 value)
    {
        AddParam(Param(name, value));
    }

    void ShaderVariation::SetUInt(const String& name, UINT32 value)
    {
        AddParam(Param(name, value));
    }

    void ShaderVariation::SetFloat(const String& name, float value)
    {
        AddParam(Param(name, value));
    }

    void ShaderVariation::SetBool(const String& name, bool value)
    {
        AddParam(Param(name, value));
    }

    Vector<String> ShaderVariation::GetParamNames() const
    {
        Vector<String> params;
        params.reserve(_params.size());

        for (auto& entry : _params)
            params.push_back(entry.first);

        return params;
    }

    ShaderDefines ShaderVariation::GetDefines() const
    {
        ShaderDefines defines;
        for (auto& entry : _params)
        {
            switch (entry.second.Type)
            {
            case ParamType::Int:
            case ParamType::Boolean:
                defines.Set(entry.first.c_str(), entry.second.I);
                break;
            case ParamType::UInt:
                defines.Set(entry.first.c_str(), entry.second.Ui);
                break;
            case ParamType::Float:
                defines.Set(entry.first.c_str(), entry.second.F);
                break;
            }
        }

        return defines;
    }

    bool ShaderVariation::Matches(const ShaderVariation& other, bool exact) const
    {
        for (auto& entry : other._params)
        {
            const auto iterFind = _params.find(entry.first);
            if (iterFind == _params.end())
                return false;

            if (entry.second.I != iterFind->second.I)
                return false;
        }

        if (exact)
        {
            for (auto& entry : _params)
            {
                const auto iterFind = other._params.find(entry.first);
                if (iterFind == other._params.end())
                    return false;

                if (entry.second.I != iterFind->second.I)
                    return false;
            }
        }

        return true;
    }

    bool ShaderVariation::operator==(const ShaderVariation& rhs) const
    {
        return Matches(rhs, true);
    }

    void ShaderVariation::ExportJson(nlohmann::json& document) const
    {
        nlohmann::json paramsJson = nlohmann::json::array();

        for (const auto& entry : _params)
        {
            nlohmann::json paramJson;
            paramJson["name"] = entry.first;
            paramJson["type"] = static_cast<uint32_t>(entry.second.Type);

            switch (entry.second.Type)
            {
            case ParamType::Int:
                paramJson["value"] = entry.second.I;
                break;
            case ParamType::UInt:
                paramJson["value"] = entry.second.Ui;
                break;
            case ParamType::Float:
                paramJson["value"] = entry.second.F;
                break;
            case ParamType::Boolean:
                paramJson["value"] = entry.second.I > 0;
                break;
            }

            paramsJson.push_back(paramJson);
        }

        document["params"] = paramsJson;
    }

    bool ShaderVariation::ImportJson(const nlohmann::json& document, ShaderVariation& variation)
    {
        if (document.is_null())
            return false;

        if (!document.contains("params") && !document.is_array() && !document.is_object())
            return false;

        const nlohmann::json* paramsJson = nullptr;
        if (document.contains("params"))
            paramsJson = &document["params"];
        else
            paramsJson = &document;

        ShaderVariation importedVariation;

        if (paramsJson->is_array())
        {
            for (const auto& paramJson : *paramsJson)
            {
                if (!paramJson.is_object() ||
                    !paramJson.contains("name") ||
                    !paramJson.contains("type") ||
                    !paramJson.contains("value"))
                {
                    return false;
                }

                if (!paramJson["name"].is_string())
                    return false;

                const String name = paramJson["name"].get<String>();
                if (!paramJson["type"].is_number_unsigned() && !paramJson["type"].is_number_integer())
                    return false;

                const auto type = static_cast<ParamType>(paramJson["type"].get<uint32_t>());
                switch (type)
                {
                case ParamType::Int:
                    if (!paramJson["value"].is_number_integer())
                        return false;
                    importedVariation.AddParam(Param(name, paramJson["value"].get<INT32>()));
                    break;
                case ParamType::UInt:
                    if (!paramJson["value"].is_number_unsigned())
                        return false;
                    importedVariation.AddParam(Param(name, paramJson["value"].get<UINT32>()));
                    break;
                case ParamType::Float:
                    if (!paramJson["value"].is_number())
                        return false;
                    importedVariation.AddParam(Param(name, paramJson["value"].get<float>()));
                    break;
                case ParamType::Boolean:
                    if (!paramJson["value"].is_boolean())
                        return false;
                    importedVariation.AddParam(Param(name, paramJson["value"].get<bool>()));
                    break;
                default:
                    return false;
                }
            }
        }
        else if (paramsJson->is_object())
        {
            for (auto& param : paramsJson->items())
            {
                const nlohmann::json& paramJson = param.value();
                if (!paramJson.is_object() ||
                    !paramJson.contains("type") ||
                    !paramJson.contains("value"))
                {
                    return false;
                }

                if (!paramJson["type"].is_number_unsigned() && !paramJson["type"].is_number_integer())
                    return false;

                const auto type = static_cast<ParamType>(paramJson["type"].get<uint32_t>());
                switch (type)
                {
                case ParamType::Int:
                    if (!paramJson["value"].is_number_integer())
                        return false;
                    importedVariation.AddParam(Param(param.key(), paramJson["value"].get<INT32>()));
                    break;
                case ParamType::UInt:
                    if (!paramJson["value"].is_number_unsigned())
                        return false;
                    importedVariation.AddParam(Param(param.key(), paramJson["value"].get<UINT32>()));
                    break;
                case ParamType::Float:
                    if (!paramJson["value"].is_number())
                        return false;
                    importedVariation.AddParam(Param(param.key(), paramJson["value"].get<float>()));
                    break;
                case ParamType::Boolean:
                    if (!paramJson["value"].is_boolean())
                        return false;
                    importedVariation.AddParam(Param(param.key(), paramJson["value"].get<bool>()));
                    break;
                default:
                    return false;
                }
            }
        }
        else
        {
            return false;
        }

        variation = importedVariation;
        return true;
    }

    void ShaderVariations::Add(const ShaderVariation& variation)
    {
        variation._idx = _nextIdx++;

        _variations.push_back(variation);
    }

    UINT32 ShaderVariations::Find(const ShaderVariation& variation) const
    {
        UINT32 idx = 0;
        for (auto& entry : _variations)
        {
            if (entry == variation)
                return idx;

            idx++;
        }

        return (UINT32)-1;
    }
}
