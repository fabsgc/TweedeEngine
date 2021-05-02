#include "TeBlendState.h"
#include "RenderAPI/TeRenderAPI.h"
#include "RenderAPI/TeRenderStateManager.h"

namespace te
{
    bool RENDER_TARGET_BLEND_STATE_DESC::operator == (const RENDER_TARGET_BLEND_STATE_DESC& rhs) const
    {
        return BlendEnable == rhs.BlendEnable &&
            SrcBlend == rhs.SrcBlend &&
            DstBlend == rhs.DstBlend &&
            BlendOp == rhs.BlendOp &&
            SrcBlendAlpha == rhs.SrcBlendAlpha &&
            DstBlendAlpha == rhs.DstBlendAlpha &&
            BlendOpAlpha == rhs.BlendOpAlpha &&
            RenderTargetWriteMask == rhs.RenderTargetWriteMask;
    }

    bool BLEND_STATE_DESC::operator == (const BLEND_STATE_DESC& rhs) const
    {
        bool equals = AlphaToCoverageEnable == rhs.AlphaToCoverageEnable &&
            IndependantBlendEnable == rhs.IndependantBlendEnable;

        if (equals)
        {
            for (UINT32 i = 0; i < TE_MAX_MULTIPLE_RENDER_TARGETS; i++)
            {
                equals &= RenderTargetDesc[i] == rhs.RenderTargetDesc[i];
            }
        }

        return equals;
    }

    BlendProperties::BlendProperties(const BLEND_STATE_DESC& desc)
        : _data(desc)
    { }

    bool BlendProperties::GetBlendEnabled(UINT32 renderTargetIdx) const
    {
        assert(renderTargetIdx < TE_MAX_MULTIPLE_RENDER_TARGETS);
        return _data.RenderTargetDesc[renderTargetIdx].BlendEnable;
    }

    BlendFactor BlendProperties::GetSrcBlend(UINT32 renderTargetIdx) const
    {
        assert(renderTargetIdx < TE_MAX_MULTIPLE_RENDER_TARGETS);
        return _data.RenderTargetDesc[renderTargetIdx].SrcBlend;
    }

    BlendFactor BlendProperties::GetDstBlend(UINT32 renderTargetIdx) const
    {
        assert(renderTargetIdx < TE_MAX_MULTIPLE_RENDER_TARGETS);
        return _data.RenderTargetDesc[renderTargetIdx].DstBlend;
    }

    BlendOperation BlendProperties::GetBlendOperation(UINT32 renderTargetIdx) const
    {
        assert(renderTargetIdx < TE_MAX_MULTIPLE_RENDER_TARGETS);
        return _data.RenderTargetDesc[renderTargetIdx].BlendOp;
    }

    BlendFactor BlendProperties::GetAlphaSrcBlend(UINT32 renderTargetIdx) const
    {
        assert(renderTargetIdx < TE_MAX_MULTIPLE_RENDER_TARGETS);
        return _data.RenderTargetDesc[renderTargetIdx].SrcBlendAlpha;
    }

    BlendFactor BlendProperties::GetAlphaDstBlend(UINT32 renderTargetIdx) const
    {
        assert(renderTargetIdx < TE_MAX_MULTIPLE_RENDER_TARGETS);
        return _data.RenderTargetDesc[renderTargetIdx].DstBlendAlpha;
    }

    BlendOperation BlendProperties::GetAlphaBlendOperation(UINT32 renderTargetIdx) const
    {
        assert(renderTargetIdx < TE_MAX_MULTIPLE_RENDER_TARGETS);
        return _data.RenderTargetDesc[renderTargetIdx].BlendOpAlpha;
    }

    UINT8 BlendProperties::GetRenderTargetWriteMask(UINT32 renderTargetIdx) const
    {
        assert(renderTargetIdx < TE_MAX_MULTIPLE_RENDER_TARGETS);
        return _data.RenderTargetDesc[renderTargetIdx].RenderTargetWriteMask;
    }

    BlendState::BlendState(const BLEND_STATE_DESC& desc)
        : _properties(desc)
    { }

    BlendState::~BlendState()
    { }

    const BlendProperties& BlendState::GetProperties() const
    {
        return _properties;
    }

    void BlendState::Initialize()
    {
        CreateInternal();
        CoreObject::Initialize();
    }

    const SPtr<BlendState>& BlendState::GetDefault()
    {
        return RenderStateManager::Instance().GetDefaultBlendState();
    }

    SPtr<BlendState> BlendState::Create(const BLEND_STATE_DESC& desc)
    {
        return RenderStateManager::Instance().CreateBlendState(desc);
    }
}
