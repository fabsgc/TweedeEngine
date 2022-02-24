#pragma once

#include "TeCorePrerequisites.h"
#include "TeCommonTypes.h"
#include "Utility/TeNonCopyable.h"

namespace te
{
    /** Data describing a texture view. */
    struct TE_CORE_EXPORT TEXTURE_VIEW_DESC
    {
        /**
         * First mip level of the parent texture the view binds (0 - base level). This applied to all array slices
         * specified below.
         */
        UINT32 MostDetailMip;

        /** Number of mip levels to bind to the view. This applied to all array slices specified below. */
        UINT32 NumMips;

        /**
         * First array slice the view binds to. This will be array index for 1D and 2D array textures, texture slice index
         * for 3D textures, and face index for cube textures(cube index * 6).
         */
        UINT32 FirstArraySlice;

        /**
         * Number of array slices to bind tot he view. This will be number of array elements for 1D and 2D array textures,
         * number of slices for 3D textures, and number of cubes for cube textures.
         */
        UINT32 NumArraySlices;

        /** Type of texture view. */
        GpuViewUsage Usage;

        /** Only used for debugging purpose */
        String DebugName = "";
    };

    /**
     * Texture views allow you to reference only a party of a texture. They may reference one or multiple mip-levels on one
     * or multiple texture array slices. Selected mip level will apply to all slices.
     *
     * They also allow you to re-purpose a texture (for example make a render target a bindable texture).
     */
    class TE_CORE_EXPORT TextureView : public NonCopyable
    {
    public:
        class HashFunction
        {
        public:
            size_t operator()(const TEXTURE_VIEW_DESC &key) const;
        };

        class EqualFunction
        {
        public:
            bool operator()(const TEXTURE_VIEW_DESC &a, const TEXTURE_VIEW_DESC &b) const;
        };

        virtual ~TextureView() = default;

        /** Returns the most detailed mip level visible by the view. */
        UINT32 GetMostDetailedMip() const { return _desc.MostDetailMip; }

        /** Returns the number of mip levels in a single slice visible by the view. */
        UINT32 GetNumMips() const { return _desc.NumMips; }

        /** Returns the first array slice index visible by this view. */
        UINT32 GetFirstArraySlice() const { return _desc.FirstArraySlice; }

        /** Returns the number of array slices visible by this view. */
        UINT32 GetNumArraySlices() const { return _desc.NumArraySlices; }

        /** Returns texture view usage. This determines where on the pipeline can be bind the view. */
        GpuViewUsage GetUsage() const { return _desc.Usage; }

        /** Returns texture view debug name */
        const String& GetDebugName() const { return _desc.DebugName; }

        /** Returns the descriptor structure used for initializing the view. */
        const TEXTURE_VIEW_DESC& GetDesc() const { return _desc; }

        /** Little trick in order to retrieve API dependent data such IShaderResourceView */
        virtual void* GetRawData() { return nullptr; }

    protected:
        TextureView(const TEXTURE_VIEW_DESC& _desc);

    protected:
        friend class Texture;

        TEXTURE_VIEW_DESC _desc;
    };
}
