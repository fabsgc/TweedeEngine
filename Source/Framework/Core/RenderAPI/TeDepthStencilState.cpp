#include "TeDepthStencilState.h"
#include "RenderAPI/TeRenderStateManager.h"

namespace te
{
    bool DEPTH_STENCIL_STATE_DESC::operator == (const DEPTH_STENCIL_STATE_DESC& rhs) const
    {
        return DepthReadEnable == rhs.DepthReadEnable &&
            DepthWriteEnable == rhs.DepthWriteEnable &&
            DepthComparisonFunc == rhs.DepthComparisonFunc &&
            StencilEnable == rhs.StencilEnable &&
            StencilReadMask == rhs.StencilReadMask &&
            StencilWriteMask == rhs.StencilWriteMask &&
            FrontStencilFailOp == rhs.FrontStencilFailOp &&
            FrontStencilZFailOp == rhs.FrontStencilZFailOp &&
            FrontStencilPassOp == rhs.FrontStencilPassOp &&
            FrontStencilComparisonFunc == rhs.FrontStencilComparisonFunc &&
            BackStencilFailOp == rhs.BackStencilFailOp &&
            BackStencilZFailOp == rhs.BackStencilZFailOp &&
            BackStencilPassOp == rhs.BackStencilPassOp &&
            BackStencilComparisonFunc == rhs.BackStencilComparisonFunc;
    }

    DepthStencilProperties::DepthStencilProperties(const DEPTH_STENCIL_STATE_DESC& desc)
        : _data(desc)
    { }

    DepthStencilState::DepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc)
        : _properties(desc)
    { }

    void DepthStencilState::Initialize()
    {
        CreateInternal();
        CoreObject::Initialize();
    }

    const SPtr<DepthStencilState>& DepthStencilState::GetDefault()
    {
        return RenderStateManager::Instance().GetDefaultDepthStencilState();
    }

    const DepthStencilProperties& DepthStencilState::GetProperties() const
    {
        return _properties;
    }

    SPtr<DepthStencilState> DepthStencilState::Create(const DEPTH_STENCIL_STATE_DESC& desc)
    {
        return RenderStateManager::Instance().CreateDepthStencilState(desc);
    }
}
