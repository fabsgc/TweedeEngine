#include "TeRasterizerState.h"
#include "TeRenderStateManager.h"

namespace te
{
    bool RASTERIZER_STATE_DESC::operator == (const RASTERIZER_STATE_DESC& rhs) const
	{
		return polygonMode == rhs.polygonMode && cullMode == rhs.cullMode;
	}

    RasterizerProperties::RasterizerProperties(const RASTERIZER_STATE_DESC& desc)
		: _data(desc)
	{ }

	RasterizerState::RasterizerState(const RASTERIZER_STATE_DESC& desc)
		: _properties(desc)
	{ }

	RasterizerState::~RasterizerState()
	{ }

    void RasterizerState::Initialize()
	{
		CreateInternal();
		CoreObject::Initialize();
	}

	const SPtr<RasterizerState>& RasterizerState::GetDefault()
	{
        return RenderStateManager::Instance().GetDefaultRasterizerState();
	}

    const RasterizerProperties& RasterizerState::GetProperties() const
	{
		return _properties;
	}

    SPtr<RasterizerState> RasterizerState::Create(const RASTERIZER_STATE_DESC& desc)
	{
        return RenderStateManager::Instance().CreateRasterizerState(desc);
	}
}
