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
        PolygonMode polygonMode = PM_SOLID;

        /**
         * Sets vertex winding order. Faces that contain vertices with this order will be culled and not rasterized. Used
         * primarily for saving cycles by not rendering backfacing faces.
         */
        CullingMode cullMode = CULL_COUNTERCLOCKWISE;

        /**
         * Represents a constant depth bias that will offset the depth values of new pixels by the specified amount.
         *
         * @note		This is useful if you want to avoid z fighting for objects at the same or similar depth.
         */
        float depthBias = 0;

        /**	Maximum depth bias value. */
        float depthBiasClamp = 0.0f;

        /**
         * Represents a dynamic depth bias that increases as the slope of the rendered polygons surface increases.
         * Resulting value offsets depth values of new pixels. This offset will be added on top of the constant depth bias.
         *
         * @note	This is useful if you want to avoid z fighting for objects at the same or similar depth.
         */
        float slopeScaledDepthBias = 0.0f;

        /**
         * If true, clipping of polygons past the far Z plane is enabled. This ensures proper Z ordering for polygons
         * outside of valid depth range (otherwise they all have the same depth). It can be useful to disable if you are
         * performing stencil operations that count on objects having a front and a back (like stencil shadow) and don't
         * want to clip the back.
         */
        bool depthClipEnable = true;

        /**
         * Scissor rectangle allows you to cull all pixels outside of the scissor rectangle.
         *
         * @see		ct::RenderAPI::setScissorRect
         */
        bool scissorEnable = false;

        /**
         * Determines how are samples in multi-sample render targets handled. If disabled all samples in the render target
         * will be written the same value, and if enabled each sample will be generated separately.
         *
         * @note	In order to get an antialiased image you need to both enable this option and use a MSAA render target.
         */
        bool multisampleEnable = true;

        /**
         * Determines should the lines be antialiased. This is separate from multi-sample antialiasing setting as lines can
         * be antialiased without multi-sampling.
         *
         * @note	This setting is usually ignored if MSAA is used, as that provides sufficient antialiasing.
         */
        bool antialiasedLineEnable = false;
    };

    /** Properties of RasterizerState. Shared between sim and core thread versions of RasterizerState. */
    class TE_CORE_EXPORT RasterizerProperties
    {
    public:
        RasterizerProperties(const RASTERIZER_STATE_DESC& desc);

        /** @copydoc RASTERIZER_STATE_DESC::polygonMode */
        PolygonMode GetPolygonMode() const { return _data.polygonMode; }

        /** @copydoc RASTERIZER_STATE_DESC::cullMode */
        CullingMode GetCullMode() const { return _data.cullMode; }

        /** @copydoc RASTERIZER_STATE_DESC::depthBias */
        float GetDepthBias() const { return _data.depthBias; }

        /** @copydoc RASTERIZER_STATE_DESC::depthBiasClamp */
        float GetDepthBiasClamp() const { return _data.depthBiasClamp; }

        /** @copydoc RASTERIZER_STATE_DESC::slopeScaledDepthBias */
        float GetSlopeScaledDepthBias() const { return _data.slopeScaledDepthBias; }

        /** @copydoc RASTERIZER_STATE_DESC::depthClipEnable */
        bool GetDepthClipEnable() const { return _data.depthClipEnable; }

        /** @copydoc RASTERIZER_STATE_DESC::scissorEnable */
        bool GetScissorEnable() const { return _data.scissorEnable; }

        /** @copydoc RASTERIZER_STATE_DESC::multisampleEnable */
        bool GetMultisampleEnable() const { return _data.multisampleEnable; }

        /** @copydoc RASTERIZER_STATE_DESC::antialiasedLineEnable */
        bool GetAntialiasedLineEnable() const { return _data.antialiasedLineEnable; }

    protected:
        friend class RasterizerState;

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
        friend class RenderStateManager;

        RasterizerState(const RASTERIZER_STATE_DESC& desc);

        /** @copydoc CoreObject::Initialize */
        void Initialize() override;

        /**	Creates any API-specific state objects. */
        virtual void CreateInternal() { }

    protected:
        RasterizerProperties _properties;
    };
}
