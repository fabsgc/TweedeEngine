#pragma once

#include "TeRenderManPrerequisites.h"
#include "Renderer/TeRenderer.h"

namespace te
{
    struct EvaluatedAnimationData;

    /** Contains information global to an entire frame. */
    struct FrameInfo
    {
        FrameInfo(const FrameTimings& timings, PerFrameData& perFrameDatas)
            : Timings(timings)
            , PerFrameDatas(perFrameDatas)
        { }

        FrameTimings Timings;
        PerFrameData& PerFrameDatas;
    };

    /** Contains pointers to all generated render textures from last frame computation */
    struct RenderTextures
    {
        SPtr<Texture> FinalTex    = nullptr;
        SPtr<Texture> ColorTex    = nullptr;
        SPtr<Texture> NormalTex   = nullptr;
        SPtr<Texture> DepthTex    = nullptr;
        SPtr<Texture> VelocityTex = nullptr;
        SPtr<Texture> EmissiveTex = nullptr;
        UnorderedMap<Light*, SPtr<Texture>> LightMaps;

        void Clear()
        {
            FinalTex = nullptr;
            ColorTex = nullptr;
            NormalTex = nullptr;
            DepthTex = nullptr;
            VelocityTex = nullptr;
            EmissiveTex = nullptr;
            LightMaps.clear();
        }
    };

    class RenderMan: public Renderer
    {
    public:
        RenderMan();
        ~RenderMan();

        /** @copydoc Renderer::Initialize */
        void Initialize() override;

        /** @copydoc Renderer::Destroy */
        void Destroy() override;

        /** @copydoc Renderer::Update */
        void Update() override;

        /** @copydoc Renderer::GetName */
        const String& GetName() const override;

        /** @copydoc Renderer::RenderAll */
        void RenderAll(PerFrameData& perFrameData) override;

        /**	Sets options used for controlling the rendering. */
        void SetOptions(const SPtr<RendererOptions>& options) override;

        /**	Returns current set of options used for controlling the rendering. */
        SPtr<RendererOptions> GetOptions() const override;

        bool RenderOverlay(RendererView& view, const FrameInfo& frameInfo);

        /** Renders all views in the provided view group. Returns true if anything has been draw to any of the views. */
        bool RenderSingleView(RendererViewGroup& viewGroup, RendererView& view, const FrameInfo& frameInfo);

        /** Renders all objects visible by the provided view. */
        void RenderSingleViewInternal(const RendererViewGroup& viewGroup, RendererView& view, const FrameInfo& frameInfo);

        /** @copydoc Renderer::NotifyCameraAdded */
        void NotifyCameraAdded(Camera* camera) override;

        /** @copydoc Renderer::NotifyCameraUpdated */
        void NotifyCameraUpdated(Camera* camera, UINT32 updateFlag) override;

        /** @copydoc Renderer::NotifyCameraRemoved */
        void NotifyCameraRemoved(Camera* camera) override;

        /** @copydoc Renderer::NotifyLightAdded */
        void NotifyLightAdded(Light* light) override;

        /** @copydoc Renderer::NotifyLightUpdated */
        void NotifyLightUpdated(Light* light) override;

        /** @copydoc Renderer::NotifyLightRemoved */
        void NotifyLightRemoved(Light* light) override;

        /** @copydoc Renderer::NotifyRenderableAdded */
        void NotifyRenderableAdded(Renderable* renderable) override;

        /** @copydoc Renderer::NotifyRenderableUpdated */
        void NotifyRenderableUpdated(Renderable* renderable) override;

        /** @copydoc Renderer::NotifyRenderableRemoved */
        void NotifyRenderableRemoved(Renderable* renderable) override;

        /** @copydoc Renderer::NotifySkyboxAdded */
        void NotifySkyboxAdded(Skybox* skybox) override;

        /** @copydoc Renderer::NotifySkyboxRemoved */
        void NotifySkyboxRemoved(Skybox* skybox) override;

        /** @copydoc Renderer::BatchRenderables */
        void BatchRenderables() override;

        /** @copydoc Renderer::SetLastRenderTexture */
        void SetLastRenderTexture(RenderOutputType type, SPtr<Texture> renderTexture) override;

        /** @copydoc Renderer::GetLastRenderTexture */
        SPtr<Texture> GetLastRenderTexture(RenderOutputType type) const override;

        /** @copydoc Renderer::SetLastLightMapTexture */
        void SetLastLightMapTexture(Light* light, SPtr<Texture> depthBuffer) override;

        /** @copydoc Renderer::GetLastLightMapTexture */
        SPtr<Texture> GetLastLightMapTexture(SPtr<Light> light) const override;

    private:
        SPtr<RendererScene> _scene;
        SPtr<RenderManOptions> _options;

        // Helpers to avoid memory allocations
        RendererViewGroup* _mainViewGroup = nullptr;

        // Keep track of all previously generated render textures
        // This structure is cleared when calling RenderAll()
        RenderTextures _renderTextures;
    };

    /** Provides easy access to the RenderBeast renderer. */
    SPtr<RenderMan> gRenderMan();
}
