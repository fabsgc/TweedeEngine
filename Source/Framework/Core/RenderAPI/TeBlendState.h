#pragma once

#include "TeCorePrerequisites.h"
#include "CoreUtility/TeCoreObject.h"

namespace te
{
    /**
     * Structure that describes blend states for a single render target. Used internally by BLEND_STATE_DESC for
     * initializing a BlendState.
     *
     * @see		BLEND_STATE_DESC
     * @see		BlendState
     */
    struct TE_CORE_EXPORT RENDER_TARGET_BLEND_STATE_DESC
    {
        bool operator==(const RENDER_TARGET_BLEND_STATE_DESC& rhs) const;

        /**
         * Queries is blending enabled for the specified render target. Blending allows you to combine the color from
         * current and previous pixel based on some value.
         */
        bool BlendEnable = false;

        /**
         * Determines what should the source blend factor be. This value determines what will the color being generated
         * currently be multiplied by.
         */
        BlendFactor SrcBlend = BF_ONE;

        /**
         * Determines what should the destination blend factor be. This value determines what will the color already in
         * render target be multiplied by.
         */
        BlendFactor DstBlend = BF_ZERO;

        /**
         * Determines how are source and destination colors combined (after they are multiplied by their respective blend
         * factors).
         */
        BlendOperation BlendOp = BO_ADD;

        /**
         * Determines what should the alpha source blend factor be. This value determines what will the alpha value being
         * generated currently be multiplied by.
         */
        BlendFactor SrcBlendAlpha = BF_ONE;

        /**
         * Determines what should the alpha destination blend factor be. This value determines what will the alpha value
         * already in render target be multiplied by.
         */
        BlendFactor DstBlendAlpha = BF_ZERO;

        /**
         * Determines how are source and destination alpha values combined (after they are multiplied by their respective
         * blend factors).
         */
        BlendOperation BlendOpAlpha = BO_ADD;

        /**
         * Render target write mask allows to choose which pixel components should the pixel shader output.
         *
         * Only the first four bits are used. First bit representing red, second green, third blue and fourth alpha value.
         * Set bits means pixel shader will output those channels.
         */
        UINT8 RenderTargetWriteMask = 0xFF;
    };
    
    /** Structure that describes render pipeline blend states. Used for initializing BlendState. */
    struct TE_CORE_EXPORT BLEND_STATE_DESC
    {
        bool operator==(const BLEND_STATE_DESC& rhs) const;

        /**
         * Alpha to coverage allows you to perform blending without needing to worry about order of rendering like regular
         * blending does. It requires multi-sampling to be active in order to work, and you need to supply an alpha texture
         * that determines object transparency.
         *
         * Blending is then performed by only using sub-samples covered by the alpha texture for the current pixel and
         * combining them with sub-samples previously stored.
         *
         * Be aware this is a limited technique only useful for certain situations. Unless you are having performance
         * problems use regular blending.
         */
        bool AlphaToCoverageEnable = false;

        /**
         * When not set, only the first render target blend descriptor will be used for all render targets. If set each
         * render target will use its own blend descriptor.
         */
        bool IndependantBlendEnable = false;

        RENDER_TARGET_BLEND_STATE_DESC RenderTargetDesc[TE_MAX_MULTIPLE_RENDER_TARGETS];
    };

    /** Properties of a BlendState. Shared between sim and core thread versions of BlendState. */
    class TE_CORE_EXPORT BlendProperties
    {
    public:
        BlendProperties(const BLEND_STATE_DESC& desc);

        /** @copydoc BLEND_STATE_DESC::alphaToCoverageEnable */
        bool GetAlphaToCoverageEnabled() const { return _data.AlphaToCoverageEnable; }

        /** @copydoc BLEND_STATE_DESC::independantBlendEnable */
        bool GetIndependantBlendEnable() const { return _data.IndependantBlendEnable; }

        /** @copydoc RENDER_TARGET_BLEND_STATE_DESC::blendEnable */
        bool GetBlendEnabled(UINT32 renderTargetIdx) const;

        /** @copydoc RENDER_TARGET_BLEND_STATE_DESC::srcBlend */
        BlendFactor GetSrcBlend(UINT32 renderTargetIdx) const;

        /** @copydoc RENDER_TARGET_BLEND_STATE_DESC::dstBlend */
        BlendFactor GetDstBlend(UINT32 renderTargetIdx) const;

        /** @copydoc RENDER_TARGET_BLEND_STATE_DESC::blendOp */
        BlendOperation GetBlendOperation(UINT32 renderTargetIdx) const;

        /** @copydoc RENDER_TARGET_BLEND_STATE_DESC::srcBlendAlpha */
        BlendFactor GetAlphaSrcBlend(UINT32 renderTargetIdx) const;

        /** @copydoc RENDER_TARGET_BLEND_STATE_DESC::dstBlendAlpha */
        BlendFactor GetAlphaDstBlend(UINT32 renderTargetIdx) const;

        /** @copydoc RENDER_TARGET_BLEND_STATE_DESC::blendOpAlpha */
        BlendOperation GetAlphaBlendOperation(UINT32 renderTargetIdx) const;

        /** @copydoc RENDER_TARGET_BLEND_STATE_DESC::renderTargetWriteMask */
        UINT8 GetRenderTargetWriteMask(UINT32 renderTargetIdx) const;

    protected:
        friend class BlendState;

        BLEND_STATE_DESC _data;
    };

    /**
     * Render system pipeline state that allows you to modify how an object is rendered. More exactly this state allows to
     * you to control how is a rendered object blended with any previously rendered objects.
     */
    class TE_CORE_EXPORT BlendState : public CoreObject
    {
    public:
        virtual ~BlendState();

        /**	Returns information about a blend state. */
        const BlendProperties& GetProperties() const;

        /**	Creates a new blend state using the specified blend state description structure. */
        static SPtr<BlendState> Create(const BLEND_STATE_DESC& desc);

        /**	Returns the default blend state that you may use when no other is available. */
        static const SPtr<BlendState>& GetDefault();

    protected:
        friend class RenderStateManager;

        BlendState(const BLEND_STATE_DESC& desc);

        /** @copydoc CoreObject::initialize */
        void Initialize() override;

        /**	Creates any API-specific state objects. */
        virtual void CreateInternal() { }

    protected:
        BlendProperties _properties;
    };
}
