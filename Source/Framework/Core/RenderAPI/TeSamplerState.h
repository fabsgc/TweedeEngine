#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeColor.h"
#include "CoreUtility/TeCoreObject.h"

#include <cfloat>

namespace te
{
    /**
     * Structure used for initializing a SamplerState.
     *
     * @see		SamplerState
     */
    struct TE_CORE_EXPORT SAMPLER_STATE_DESC
    {
        SAMPLER_STATE_DESC() { };

        bool operator==(const SAMPLER_STATE_DESC& rhs) const;

        /** Determines how are texture coordinates outside of [0, 1] range handled. */
        UVWAddressingMode AddressMode;

        /** Filtering used when texture is displayed as smaller than its original size. */
        FilterOptions MinFilter = FO_LINEAR;

        /** Filtering used when texture is displayed as larger than its original size. */
        FilterOptions MagFilter = FO_LINEAR;

        /** Filtering used to blend between the different mip levels. */
        FilterOptions MipFilter = FO_LINEAR;

        /** Maximum number of samples if anisotropic filtering is enabled. Max is 16. */
        UINT32 MaxAnisotropy = 0;

        /**
         * Mipmap bias allows you to adjust the mipmap selection calculation. Negative values  force a larger mipmap to be
         * used, and positive values smaller. Units are in values of mip levels, so -1 means use a mipmap one level higher
         * than default.
         */
        float MipmapBias = 0;

        /** Minimum mip-map level that is allowed to be displayed. */
        float MipMin = -FLT_MAX;

        /** Maximum mip-map level that is allowed to be displayed. Set to FLT_MAX for no limit. */
        float MipMax = FLT_MAX;

        /** Border color to use when using border addressing mode as specified by @p addressMode. */
        Color BorderColor = Color::White;

        /** Function that compares sampled data with existing sampled data. */
        CompareFunction ComparisonFunc = CMPF_ALWAYS_PASS;
    };
    
    /** Properties of SamplerState. Shared between sim and core thread versions of SamplerState. */
    class TE_CORE_EXPORT SamplerProperties
    {
    public:
        SamplerProperties(const SAMPLER_STATE_DESC& desc);

        /**
         * Returns texture addressing mode for each possible texture coordinate. Addressing modes determine how are texture
         * coordinates outside of [0, 1] range handled.
         */
        const UVWAddressingMode& GetTextureAddressingMode() const { return _data.AddressMode; }

        /** Gets the filtering used when sampling from a texture. */
        FilterOptions GetTextureFiltering(FilterType ftpye) const;

        /**
         * Gets the anisotropy level. Higher anisotropy means better filtering for textures displayed on an angled slope
         * relative to the viewer.
         */
        unsigned int GetTextureAnisotropy() const { return _data.MaxAnisotropy; }

        /** Gets a function that compares sampled data with existing sampled data. */
        CompareFunction GetComparisonFunction() const { return _data.ComparisonFunc; }

        /**
         * Mipmap bias allows you to adjust the mipmap selection calculation. Negative values  force a larger mipmap to be
         * used, and positive values smaller. Units are in values of mip levels, so -1 means use a mipmap one level higher
         * than default.
         */
        float GetTextureMipmapBias() const { return _data.MipmapBias; }

        /** Returns the minimum mip map level. */
        float GetMinimumMip() const { return _data.MipMin; }

        /** Returns the maximum mip map level. */
        float GetMaximumMip() const { return _data.MipMax; }

        /**
         * Gets the border color that will be used when border texture addressing is used and texture address is outside of
         * the valid range.
         */
        const Color& GetBorderColor() const;

        /**	Returns the descriptor originally used for creating the sampler state. */
        SAMPLER_STATE_DESC GetDesc() const { return _data; }

    protected:
        friend class SamplerState;

        SAMPLER_STATE_DESC _data;
    };

    /**
     * Class representing the state of a texture sampler.
     *
     * @note Sampler units are used for retrieving and filtering data from textures set in a GPU program. Sampler states are
     */
    class TE_CORE_EXPORT SamplerState : public CoreObject
    {
    public:
        virtual ~SamplerState() = default;

        /**	Returns information about the sampler state. */
        const SamplerProperties& GetProperties() const;

        /**	Creates a new sampler state using the provided descriptor structure. */
        static SPtr<SamplerState> Create(const SAMPLER_STATE_DESC& desc);

        /**	Returns the default sampler state. */
        static const SPtr<SamplerState>& GetDefault();

    protected:
        friend class RenderStateManager;

        SamplerState(const SAMPLER_STATE_DESC& desc);

        /** @copydoc CoreObject::initialize */
        void Initialize() override;

        /**	Creates any API-specific state objects. */
        virtual void CreateInternal() { }

    protected:
        SamplerProperties _properties;
    };
}
