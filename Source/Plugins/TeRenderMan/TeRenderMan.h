#pragma once

#include "TeRenderManPrerequisites.h"
#include "Renderer/TeRenderer.h"
#include "TeRenderManOptions.h"
#include "TeRendererView.h"
#include "TeRendererScene.h"

namespace te
{
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

        void RenderOverlay(const SPtr<RenderTarget> target, Camera* camera);

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

    private:
        SPtr<RendererScene> _scene;
        SPtr<RenderManOptions> _options;
    };

    /** Provides easy access to the RenderBeast renderer. */
    SPtr<RenderMan> gRenderMan();
}
