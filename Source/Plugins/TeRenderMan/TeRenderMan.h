#pragma once

#include "TeRenderManPrerequisites.h"
#include "Renderer/TeRenderer.h"

namespace te
{   
    /** Contains information global to an entire frame. */
    struct FrameInfo
    {
        FrameInfo(const FrameTimings& timings)
            :timings(timings)
        { }

        FrameTimings timings;
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

        void RenderOverlay(const SPtr<RenderTarget> target, Camera* camera);

        /** @copydoc Renderer::NotifyCameraAdded */
        void NotifyCameraAdded(Camera* camera) override;

        /** @copydoc Renderer::NotifyCameraUpdated */
        void NotifyCameraUpdated(Camera* camera) override;

        /** @copydoc Renderer::NotifyCameraRemoved */
        void NotifyCameraRemoved(Camera* camera) override;

    private:
        // Scene data
        SPtr<RendererScene> _scene;

        SPtr<RenderManOptions> _options;
    };

    /**	Provides easy access to the RenderBeast renderer. */
    SPtr<RenderMan> gRenderMan();

}
