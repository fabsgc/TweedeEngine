#pragma once

#include "TeCorePrerequisites.h"
#include "CoreUtility/TeCoreObject.h"

namespace te
{
    /**
     * Descriptor structured used for initializing DepthStencilState.
     *
     * @see DepthStencilState
     */
    struct TE_CORE_EXPORT DEPTH_STENCIL_STATE_DESC
    {
        bool operator==(const DEPTH_STENCIL_STATE_DESC& rhs) const;

        /**
         * If enabled, any pixel about to be written will be tested against the depth value currently in the buffer. If the
         * depth test passes (depending on the set valueand chosen depth comparison function), that pixel is written and
         * depth is updated (if depth write is enabled).
         */
        bool DepthReadEnable = true;

        /** If enabled rendering pixels will update the depth buffer value. */
        bool DepthWriteEnable = true;

        /**
         * Determines what operation should the renderer use when comparing previous and current depth value. If the
         * operation passes, pixel with the current depth value will be considered visible.
         */
        CompareFunction DepthComparisonFunc = CMPF_LESS_EQUAL;

        /**
         * If true then stencil buffer will also be updated when a pixel is written, and pixels will be tested against
         * the stencil buffer before rendering.
         */
        bool StencilEnable = false;

        /** Mask to apply to any value read from the stencil buffer, before applying the stencil comparison function. */
        UINT8 StencilReadMask = 0xFF;

        /** Mask to apply to any value about to be written in the stencil buffer. */
        UINT8 StencilWriteMask = 0xFF;

        /** Operation that happens when stencil comparison function fails on a front facing polygon. */
        StencilOperation FrontStencilFailOp = SOP_KEEP;

        /** Operation that happens when stencil comparison function passes but depth test fails on a front facing polygon. */
        StencilOperation FrontStencilZFailOp = SOP_KEEP;

        /** Operation that happens when stencil comparison function passes on a front facing polygon. */
        StencilOperation FrontStencilPassOp = SOP_KEEP;

        /**
         * Stencil comparison function used for front facing polygons. Stencil buffer will be modified according to
         * previously set stencil operations depending whether this comparison passes or fails.
         */
        CompareFunction FrontStencilComparisonFunc = CMPF_ALWAYS_PASS;

        /** Operation that happens when stencil comparison function fails on a back facing polygon. */
        StencilOperation BackStencilFailOp = SOP_KEEP;

        /** Operation that happens when stencil comparison function passes but depth test fails on a back facing polygon. */
        StencilOperation BackStencilZFailOp = SOP_KEEP;

        /** Operation that happens when stencil comparison function passes on a back facing polygon. */
        StencilOperation BackStencilPassOp = SOP_KEEP;

        /**
         * Stencil comparison function used for back facing polygons. Stencil buffer will be modified according to
         * previously set stencil operations depending whether this comparison passes or fails.
         */
        CompareFunction BackStencilComparisonFunc = CMPF_ALWAYS_PASS;
    };

    class TE_CORE_EXPORT DepthStencilProperties
    {
    public:
        DepthStencilProperties(const DEPTH_STENCIL_STATE_DESC& desc);
        
        /** @copydoc DEPTH_STENCIL_STATE_DESC::depthReadEnable */
        bool GetDepthReadEnable() const { return _data.DepthReadEnable; }

        /** @copydoc DEPTH_STENCIL_STATE_DESC::depthWriteEnable */
        bool GetDepthWriteEnable() const { return _data.DepthWriteEnable; }

        /** @copydoc DEPTH_STENCIL_STATE_DESC::depthComparisonFunc */
        CompareFunction GetDepthComparisonFunc() const { return _data.DepthComparisonFunc; }

        /** @copydoc DEPTH_STENCIL_STATE_DESC::stencilEnable */
        bool GetStencilEnable() const { return _data.StencilEnable; }

        /** @copydoc DEPTH_STENCIL_STATE_DESC::stencilReadMask */
        UINT8 GetStencilReadMask() const { return _data.StencilReadMask; }

        /** @copydoc DEPTH_STENCIL_STATE_DESC::stencilWriteMask */
        UINT8 GetStencilWriteMask() const { return _data.StencilWriteMask; }

        /** @copydoc DEPTH_STENCIL_STATE_DESC::frontStencilFailOp */
        StencilOperation GetStencilFrontFailOp() const { return _data.FrontStencilFailOp; }

        /** @copydoc DEPTH_STENCIL_STATE_DESC::frontStencilZFailOp */
        StencilOperation GetStencilFrontZFailOp() const { return _data.FrontStencilZFailOp; }

        /** @copydoc DEPTH_STENCIL_STATE_DESC::frontStencilPassOp */
        StencilOperation GetStencilFrontPassOp() const { return _data.FrontStencilPassOp; }

        /** @copydoc DEPTH_STENCIL_STATE_DESC::frontStencilComparisonFunc */
        CompareFunction GetStencilFrontCompFunc() const { return _data.FrontStencilComparisonFunc; }

        /** @copydoc DEPTH_STENCIL_STATE_DESC::backStencilFailOp */
        StencilOperation GetStencilBackFailOp() const { return _data.BackStencilFailOp; }

        /** @copydoc DEPTH_STENCIL_STATE_DESC::backStencilZFailOp */
        StencilOperation GetStencilBackZFailOp() const { return _data.BackStencilZFailOp; }

        /** @copydoc DEPTH_STENCIL_STATE_DESC::backStencilPassOp */
        StencilOperation GetStencilBackPassOp() const { return _data.BackStencilPassOp; }

        /** @copydoc DEPTH_STENCIL_STATE_DESC::backStencilComparisonFunc */
        CompareFunction GetStencilBackCompFunc() const { return _data.BackStencilComparisonFunc; }

    protected:
        friend class DepthStencilState;

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

        /** Returns information about the depth stencil state. */
        const DepthStencilProperties& GetProperties() const;

        /** Creates a new depth stencil state using the specified depth stencil state description structure. */
        static SPtr<DepthStencilState> Create(const DEPTH_STENCIL_STATE_DESC& desc);

        /** Returns the default depth stencil state that you may use when no other is available. */
        static const SPtr<DepthStencilState>& GetDefault();
    
    protected:
        friend class RenderStateManager;

        DepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc);

        /** @copydoc CoreObject::initialize */
        void Initialize() override;

        /** Creates any API-specific state objects. */
        virtual void CreateInternal() { }

    protected:
        DepthStencilProperties _properties;
    };
}
