#pragma once

#include "TeRenderManPrerequisites.h"
#include "Renderer/TeRenderer.h"
#include "TeRenderManOptions.h"
#include "TeRendererView.h"
#include "TeRendererScene.h"

namespace te
{
    /** Contains information global to an entire frame. */
    struct FrameInfo
    {
        FrameInfo(const FrameTimings& timings)
            : Timings(timings)
        { }

        FrameTimings Timings;
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
        void RenderAll() override;

        /**	Sets options used for controlling the rendering. */
        void SetOptions(const SPtr<RendererOptions>& options) override;

        /**	Returns current set of options used for controlling the rendering. */
        SPtr<RendererOptions> GetOptions() const override;

        bool RenderOverlay(RendererView& view, const FrameInfo& frameInfo);

        /** Renders all views in the provided view group. Returns true if anything has been draw to any of the views. */
        bool RenderViews(RendererViewGroup& viewGroup, const FrameInfo& frameInfo);

        /** Renders all objects visible by the provided view. */
        void RenderSingleView(const RendererViewGroup& viewGroup, RendererView& view, const FrameInfo& frameInfo);

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

    private:
        SPtr<RendererScene> _scene;
        SPtr<RenderManOptions> _options;

        // Helpers to avoid memory allocations
        RendererViewGroup* _mainViewGroup = nullptr;
    };

    /** Provides easy access to the RenderBeast renderer. */
    SPtr<RenderMan> gRenderMan();
}
