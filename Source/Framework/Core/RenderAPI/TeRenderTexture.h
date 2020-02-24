#pragma once

#include "TeCorePrerequisites.h"
#include "Image/TeTexture.h"
#include "RenderAPI/TeRenderTarget.h"

namespace te
{
    /** Structure that describes a render texture color and depth/stencil surfaces. */
    struct TE_CORE_EXPORT RENDER_TEXTURE_DESC
    {
        RENDER_SURFACE_DESC ColorSurfaces[TE_MAX_MULTIPLE_RENDER_TARGETS];
        RENDER_SURFACE_DESC DepthStencilSurface;
    };

    /** Contains various properties that describe a render texture. */
    class TE_CORE_EXPORT RenderTextureProperties : public RenderTargetProperties
    {
    public:
        RenderTextureProperties(const RENDER_TEXTURE_DESC& desc, bool requiresFlipping);
        virtual ~RenderTextureProperties() { }

    private:
        void Construct(const TextureProperties* textureProps, UINT32 numSlices, UINT32 mipLevel, bool requiresFlipping, bool hwGamma);

        friend class RenderTexture;
    };

    /**
     * Render target specialization that allows you to render into one or multiple textures. Such textures can then be used
     * in other operations as GPU program input.
     *
     * @note Sim thread only. Retrieve core implementation from getCore() for core thread only functionality.
     */
    class TE_CORE_EXPORT RenderTexture : public RenderTarget
    {
    public:
        RenderTexture(const RENDER_TEXTURE_DESC& desc, UINT32 deviceIdx);
        virtual ~RenderTexture() {}

        /** @copydoc CoreObject::Initialize */
        void Initialize() override;

        /** @copydoc TextureManager::CreateRenderTexture(const TEXTURE_DESC&, bool, PixelFormat) */
        static SPtr<RenderTexture> Create(const TEXTURE_DESC& colorDesc,
            bool createDepth = true, PixelFormat depthStencilFormat = PF_D32);

        /** @copydoc TextureManager::CreateRenderTexture(const RENDER_TEXTURE_DESC&) */
        static SPtr<RenderTexture> Create(const RENDER_TEXTURE_DESC& desc);

        /**
         * Returns a color surface texture you may bind as an input to an GPU program.
         *
         * @note Be aware that you cannot bind a render texture for reading and writing at the same time.
         */
        SPtr<Texture>& GetColorTexture(UINT32 idx) { return _desc.ColorSurfaces[idx].Tex; }

        /**
         * Returns a depth/stencil surface texture you may bind as an input to an GPU program.
         *
         * @note Be aware that you cannot bind a render texture for reading and writing at the same time.
         */
        SPtr<Texture>& GetDepthStencilTexture() { return _desc.DepthStencilSurface.Tex; }

    protected:
    private:
        /** Throws an exception of the color and depth/stencil buffers aren't compatible. */
        void ThrowIfBuffersDontMatch() const;

    protected:
        SPtr<TextureView> _colorSurfaces[TE_MAX_MULTIPLE_RENDER_TARGETS];
        SPtr<TextureView> _depthStencilSurface;

        RENDER_TEXTURE_DESC _desc;

    };
}
