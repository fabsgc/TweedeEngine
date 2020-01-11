#include "TeDepthStencilState.h"
#include "RenderAPI/TeRenderStateManager.h"

namespace te
{
    DepthStencilProperties::DepthStencilProperties(const DEPTH_STENCIL_STATE_DESC& desc)
		: _data(desc)
    {
	}

	DepthStencilState::DepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc)
		: _properties(desc)
	{
	}

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
