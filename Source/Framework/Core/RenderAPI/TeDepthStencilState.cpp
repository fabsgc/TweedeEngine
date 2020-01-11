#include "TeDepthStencilState.h"

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
        // TODO
		CreateInternal();
		CoreObject::Initialize();
	}

    const SPtr<DepthStencilState>& DepthStencilState::GetDefault()
	{
        // TODO
		return DepthStencilState::Create(DEPTH_STENCIL_STATE_DESC());
	}

    const DepthStencilProperties& DepthStencilState::GetProperties() const
	{
		return _properties;
	}

    SPtr<DepthStencilState> DepthStencilState::Create(const DEPTH_STENCIL_STATE_DESC& desc)
	{
        DepthStencilState* depthStencilState = new (te_allocate<DepthStencilState>()) DepthStencilState(desc);
        SPtr<DepthStencilState> handlerPtr = te_shared_ptr<DepthStencilState>(depthStencilState);
        handlerPtr->SetThisPtr(handlerPtr);

		return handlerPtr;
	}
}
