#pragma once

#include "TeCorePrerequisites.h"
#include "CoreUtility/TeCoreObject.h"

namespace te
{
    /**
	 * Descriptor structured used for initializing DepthStencilState.
	 *
	 * @see		DepthStencilState
	 */
	struct TE_CORE_EXPORT DEPTH_STENCIL_STATE_DESC
	{
        // TODO
    };

    class TE_CORE_EXPORT DepthStencilProperties
	{
    public:
        DepthStencilProperties(const DEPTH_STENCIL_STATE_DESC& desc);
        // TODO

    protected:
        DEPTH_STENCIL_STATE_DESC _data;
    };

    /**
	 * Render system pipeline state that allows you to modify how an object is rendered. More exactly this state allows to
	 * you to control how are depth and stencil buffers modified upon rendering.
	 */
	class TE_CORE_EXPORT DepthStencilState : public CoreObject
	{
	public:
		virtual ~DepthStencilState() = default;

		/**	Returns information about the depth stencil state. */
		const DepthStencilProperties& GetProperties() const;

		/**	Creates a new depth stencil state using the specified depth stencil state description structure. */
		static SPtr<DepthStencilState> Create(const DEPTH_STENCIL_STATE_DESC& desc);

		/**	Returns the default depth stencil state that you may use when no other is available. */
		static const SPtr<DepthStencilState>& GetDefault();
    
    protected:
		friend class RenderStateManager;

        DepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc);

		/** @copydoc CoreObject::initialize */
        void Initialize() override;

		/**	Creates any API-specific state objects. */
		virtual void CreateInternal() { }

    protected:
        DepthStencilProperties _properties;
    };
}
