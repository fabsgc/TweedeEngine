#pragma once

#include "TeRenderManPrerequisites.h"
#include "Renderer/TeRenderer.h"

namespace te
{
    struct EvaluatedAnimationData;

    /** Contains information global to an entire frame. */
    struct FrameInfo
    {
        FrameInfo(const FrameTimings& timings, FrameData& frameDatas)
            : Timings(timings)
            , FrameDatas(frameDatas)
        { }

        FrameTimings Timings;
        FrameData& FrameDatas;
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
        SPtr<Texture> SSAOTex     = nullptr;
        UnorderedMap<Light*, SPtr<Texture>> LightMaps;

        void Clear()
        {
            FinalTex = nullptr;
            ColorTex = nullptr;
            NormalTex = nullptr;
            DepthTex = nullptr;
            VelocityTex = nullptr;
            EmissiveTex = nullptr;
            SSAOTex = nullptr;
            LightMaps.clear();
        }
    };

    class RenderMan: public Renderer
    {
    public:
        RenderMan();
        virtual ~RenderMan();

        /** @copydoc Renderer::Initialize */
        void Initialize() override;

        /** @copydoc Renderer::Destroy */
        void Destroy() override;

        /** @copydoc Renderer::Update */
        void Update() override;

        /** @copydoc Renderer::GetName */
        const String& GetName() const override;

        /** @copydoc Renderer::RenderAll */
        void RenderAll(FrameData& frameData) override;

        /**	Sets options used for controlling the rendering. */
        void SetOptions(const SPtr<RendererOptions>& options) override;

        /**	Returns current set of options used for controlling the rendering. */
        SPtr<RendererOptions> GetOptions() const override;

        /** Renders all views in the provided view group. Returns true if anything has been draw to any of the views. */
        bool RenderViews(RendererViewGroup& viewGroup, const FrameInfo& frameInfo);

        /** Renders all overlay of the provided view. Returns true if anything has been rendered in any of the views. */
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

        /** @copydoc Renderer::NotifyCamerasCleared */
        void NotifyCamerasCleared() override;

        /** @copydoc Renderer::NotifyLightAdded */
        void NotifyLightAdded(Light* light) override;

        /** @copydoc Renderer::NotifyLightUpdated */
        void NotifyLightUpdated(Light* light, UINT32 updateFlag) override;

        /** @copydoc Renderer::NotifyLightRemoved */
        void NotifyLightRemoved(Light* light) override;

        /** @copydoc Renderer::NotifyLightsCleared */
        void NotifyLightsCleared() override;

        /** @copydoc Renderer::NotifyRenderableAdded */
        void NotifyRenderableAdded(Renderable* renderable) override;

        /** @copydoc Renderer::NotifyRenderableUpdated */
        void NotifyRenderableUpdated(Renderable* renderable, UINT32 updateFlag) override;

        /** @copydoc Renderer::NotifyRenderableRemoved */
        void NotifyRenderableRemoved(Renderable* renderable) override;

        /** @copydoc Renderer::NotifyRenderablesCleared */
        void NotifyRenderablesCleared() override;

        /** @copydoc Renderer::NotifySkyboxAdded */
        void NotifySkyboxAdded(Skybox* skybox) override;

        /** @copydoc Renderer::NotifySkyboxRemoved */
        void NotifySkyboxRemoved(Skybox* skybox) override;

        /** @copydoc Renderer::NotifySkyboxCleared */
        void NotifySkyboxCleared() override;

        /** @copydoc Renderer::NotifyDecalAdded */
        void NotifyDecalAdded(Decal* decal) override;

        /** @copydoc Renderer::NotifyDecalUpdated */
        void NotifyDecalUpdated(Decal* decal, UINT32 updateFlag) override;

        /** @copydoc Renderer::NotifyDecalRemoved */
        void NotifyDecalRemoved(Decal* decal) override;

        /** @copydoc Renderer::NotifyDecalsCleared */
        void NotifyDecalsCleared() override;

        /** @copydoc Renderer::BatchRenderables */
        void BatchRenderables() override;

        /** @copydoc Renderer::DestroyBatchedRenderables */
        void DestroyBatchedRenderables() override;

        /** @copydoc Renderer::SetLastRenderTexture */
        void SetLastRenderTexture(RenderOutputType type, SPtr<Texture> renderTexture) const override;

        /** @copydoc Renderer::GetLastRenderTexture */
        SPtr<Texture> GetLastRenderTexture(RenderOutputType type) const override;

        /** @copydoc Renderer::SetLastShadowMapTexture */
        void SetLastShadowMapTexture(Light* light, SPtr<Texture> depthBuffer) const override;

        /** @copydoc Renderer::GetLastShadowMapTexture */
        SPtr<Texture> GetLastShadowMapTexture(SPtr<Light> light) const override;

    private:
        SPtr<RendererScene> _scene;
        SPtr<RenderManOptions> _options;

        // Helpers to avoid memory allocations
        RendererViewGroup* _mainViewGroup = nullptr;

        // Keep track of all previously generated render textures
        // This structure is cleared when calling RenderAll()
        mutable RenderTextures _renderTextures;

        // Keep a reference to RenderAPI instance as 
        // RenderAPI::Instance can be expensive if called to often
        RenderAPI& _renderAPI;
    };

    /** Provides easy access to the RenderBeast renderer. */
    SPtr<RenderMan> gRenderMan();
}
