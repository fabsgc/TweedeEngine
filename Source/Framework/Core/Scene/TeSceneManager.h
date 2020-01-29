#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeEvent.h"

namespace te
{
    /**
     * Keeps track of all active SceneObject%s and their components. Keeps track of component state and triggers their
     * events. Updates the transforms of objects as SceneObject%s move.
     */
    class TE_CORE_EXPORT SceneManager : public Module<SceneManager>
    {
    public:
        SceneManager();
        ~SceneManager();

        void Initialize();

        /** Notifies the scene manager that a new camera was created. */
        void RegisterCamera(const SPtr<Camera>& camera);

        /** Notifies the scene manager that a camera was removed. */
        void UnregisterCamera(const SPtr<Camera>& camera);

        /**
         * Sets the render target that the main camera in the scene (if any) will render its view to. This generally means
         * the main game window when running standalone, or the Game viewport when running in editor.
         */
        void SetMainRenderTarget(const SPtr<RenderTarget>& rt);

    protected:
        /**	Callback that is triggered when the main render target size is changed. */
        void OnMainRenderTargetResized();

    protected:
        UnorderedMap<Camera*, SPtr<Camera>> _cameras;
        SPtr<RenderTarget> _mainRenderTarget;

        HEvent _mainRTResizedConn;
    };

    /** Provides easy access to the SceneManager. */
    TE_CORE_EXPORT SceneManager& gSceneManager();
}
