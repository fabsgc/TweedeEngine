#include "TeRasterizerState.h"

namespace te
{
    bool RASTERIZER_STATE_DESC::operator == (const RASTERIZER_STATE_DESC& rhs) const
	{
		return PolygonMode == rhs.PolygonMode && CullMode == rhs.CullMode;
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
        return RasterizerState::Create(RASTERIZER_STATE_DESC());
	}

    const RasterizerProperties& RasterizerState::GetProperties() const
	{
		return _properties;
	}

    SPtr<RasterizerState> RasterizerState::Create(const RASTERIZER_STATE_DESC& desc)
	{
        RasterizerState* rasterizeState = new (te_allocate<RASTERIZER_STATE_DESC>()) RasterizerState(desc);
        SPtr<RasterizerState> handlerPtr = te_shared_ptr<RasterizerState>(rasterizeState);
        handlerPtr->SetThisPtr(handlerPtr);

		return handlerPtr;
	}
}
