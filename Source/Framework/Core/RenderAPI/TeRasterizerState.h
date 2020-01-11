#pragma once

#include "TeCorePrerequisites.h"
#include "CoreUtility/TeCoreObject.h"

namespace te
{
    /** Structure that describes pipeline rasterizer state. Used for initializing a RasterizerState. */
	struct TE_CORE_EXPORT RASTERIZER_STATE_DESC
	{
        bool operator==(const RASTERIZER_STATE_DESC& rhs) const;

        /** Polygon mode allows you to draw polygons as solid objects or as wireframe by just drawing their edges. */
		PolygonMode PolygonMode = PM_SOLID;

		/**
		 * Sets vertex winding order. Faces that contain vertices with this order will be culled and not rasterized. Used
		 * primarily for saving cycles by not rendering backfacing faces.
		 */
		CullingMode CullMode = CULL_COUNTERCLOCKWISE;

        // TODO
    };

    /** Properties of RasterizerState. Shared between sim and core thread versions of RasterizerState. */
	class TE_CORE_EXPORT RasterizerProperties
	{
	public:
		RasterizerProperties(const RASTERIZER_STATE_DESC& desc);

		/** @copydoc RASTERIZER_STATE_DESC::PolygonMode */
		PolygonMode GetPolygonMode() const { return _data.PolygonMode; }

		/** @copydoc RASTERIZER_STATE_DESC::CullMode */
		CullingMode GetCullMode() const { return _data.CullMode; }

    protected:
        RASTERIZER_STATE_DESC _data;

    };

    /**
	 * Render system pipeline state that allows you to modify how an object is rasterized (how are polygons converted
	 * to pixels).
	 */
	class TE_CORE_EXPORT RasterizerState : public CoreObject
	{
	public:
        virtual ~RasterizerState();

		/**	Returns information about the rasterizer state. */
		const RasterizerProperties& GetProperties() const;

        /** Creates a new rasterizer state using the specified rasterizer state descriptor structure. */
		static SPtr<RasterizerState> Create(const RASTERIZER_STATE_DESC& desc);

		/**	Returns the default rasterizer state. */
		static const SPtr<RasterizerState>& GetDefault();

    protected:
        RasterizerState(const RASTERIZER_STATE_DESC& desc);

        /** @copydoc CoreObject::Initialize */
		void Initialize() override;

		/**	Creates any API-specific state objects. */
		virtual void CreateInternal() { }

    protected:
        RasterizerProperties _properties;
    };
}
