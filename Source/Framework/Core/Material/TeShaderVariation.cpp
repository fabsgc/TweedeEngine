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

    ShaderVariation::ShaderVariation()
        : Serializable(TID_ShaderVariation)
    { }

    ShaderVariation::ShaderVariation(const Vector<Param>& params)
        : Serializable(TID_ShaderVariation)
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
            case Int:
            case Bool:
                defines.Set(entry.first.c_str(), entry.second.I);
                break;
            case UInt:
                defines.Set(entry.first.c_str(), entry.second.Ui);
                break;
            case Float:
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
