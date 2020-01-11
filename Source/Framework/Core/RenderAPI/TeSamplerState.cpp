#include "RenderAPI/TeSamplerState.h"
#include "RenderAPI/TeRenderStateManager.h"

namespace te
{
    bool SAMPLER_STATE_DESC::operator == (const SAMPLER_STATE_DESC& rhs) const
    {
        return AddressMode == rhs.AddressMode &&
            MinFilter == rhs.MinFilter &&
            MagFilter == rhs.MagFilter &&
            MipFilter == rhs.MipFilter &&
            MaxAniso == rhs.MaxAniso &&
            MipmapBias == rhs.MipmapBias &&
            MipMin == rhs.MipMin &&
            MipMax == rhs.MipMax &&
            BorderColor == rhs.BorderColor &&
            ComparisonFunc == rhs.ComparisonFunc;
    }

    SamplerProperties::SamplerProperties(const SAMPLER_STATE_DESC& desc)
        : _data(desc)
    { }

    FilterOptions SamplerProperties::GetTextureFiltering(FilterType ft) const
    {
        switch (ft)
        {
        case FT_MIN:
            return _data.MinFilter;
        case FT_MAG:
            return _data.MagFilter;
        case FT_MIP:
            return _data.MipFilter;
        }

        return _data.MinFilter;
    }

    const Color& SamplerProperties::GetBorderColor() const
    {
        return _data.BorderColor;
    }

    SamplerState::SamplerState(const SAMPLER_STATE_DESC& desc)
        : _properties(desc)
    { }

    void SamplerState::Initialize()
    {
        CreateInternal();
        CoreObject::Initialize();
    }

    const SPtr<SamplerState>& SamplerState::GetDefault()
    {
        return RenderStateManager::Instance().GetDefaultSamplerState();
    }

    const SamplerProperties& SamplerState::GetProperties() const
    {
        return _properties;
    }

    SPtr<SamplerState> SamplerState::Create(const SAMPLER_STATE_DESC& desc)
    {
        return RenderStateManager::Instance().CreateSamplerState(desc);
    }
}
