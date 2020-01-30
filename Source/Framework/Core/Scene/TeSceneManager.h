#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeEvent.h"
#include "TeSceneObject.h"

namespace te
{
    /** Possible states components can be in. Controls which component callbacks are triggered. */
    enum class ComponentState
    {
        Running, /**< All components callbacks are being triggered normally. */
        Paused, /**< All component callbacks except update are being triggered normally. */
        Stopped /**< No component callbacks are being triggered. */
    };

    /** Information about a scene actor and the scene object it has been bound to. */
    struct BoundActorData
    {
        BoundActorData() = default;
        BoundActorData(const SPtr<SceneActor>& actor, const HSceneObject& so)
            : Actor(actor)
            , So(so)
        { }

        SPtr<SceneActor> Actor;
        HSceneObject So;
    };

    /** Contains information about an instantiated scene. */
    class TE_CORE_EXPORT SceneInstance
    {
    public:
        SceneInstance(const String& name, const HSceneObject& root);

        /** Name of the scene. */
        const String& GetName() const { return _name; }

        /** Root object of the scene. */
        const HSceneObject& GetRoot() const { return _root; }

        /** Checks is the scene currently active. IF inactive the scene properties aside from the name are undefined. */
        bool isActive() const { return _isActive; }

    private:
        friend class SceneManager;

        String _name;
        HSceneObject _root;
        bool _isActive = true;
    };

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

        /** Returns the object that represents the main scene. */
        const SPtr<SceneInstance>& GetMainScene() const { return _mainScene; }

        /** Notifies the scene manager that a new camera was created. */
        void RegisterCamera(const SPtr<Camera>& camera);

        /** Notifies the scene manager that a camera was removed. */
        void UnregisterCamera(const SPtr<Camera>& camera);

        /**
         * Register a new node in the scene manager, on the top-most level of the hierarchy.
         *
         * @param[in]	node	Node you wish to add. It's your responsibility not to add duplicate or null nodes. This
         *						method won't check.
         *
         * @note
         * After you add a node in the scene manager, it takes ownership of its memory and is responsible for releasing it.
         * Do NOT add nodes that have already been added (if you just want to change their parent). Normally this
         * method will only be called by SceneObject.
         */
        void RegisterNewSO(const HSceneObject& node);

        /** Notifies the manager that a new component has just been created. The manager triggers necessary callbacks. */
        void _notifyComponentCreated(const HComponent& component, bool parentActive);

        /**
         * Notifies the manager that a scene object the component belongs to was activated. The manager triggers necessary
         * callbacks.
         */
        void _notifyComponentActivated(const HComponent& component, bool triggerEvent);

        /**
         * Notifies the manager that a scene object the component belongs to was deactivated. The manager triggers necessary
         * callbacks.
         */
        void _notifyComponentDeactivated(const HComponent& component, bool triggerEvent);

        /** Notifies the manager that a component is about to be destroyed. The manager triggers necessary callbacks. */
        void _notifyComponentDestroyed(const HComponent& component, bool immediate);

        /**
         * Changes the component state that globally determines which component callbacks are activated. Only affects
         * components that don't have the ComponentFlag::AlwaysRun flag set.
         */
        void SetComponentState(ComponentState state);

        /** Checks are the components currently in the Running state. */
        bool IsRunning() const { return _componentState == ComponentState::Running; }

        /**
         * Sets the render target that the main camera in the scene (if any) will render its view to. This generally means
         * the main game window when running standalone, or the Game viewport when running in editor.
         */
        void SetMainRenderTarget(const SPtr<RenderTarget>& rt);

    protected:
        /**	Callback that is triggered when the main render target size is changed. */
        void OnMainRenderTargetResized();

    protected:
        SPtr<SceneInstance> _mainScene;

        UnorderedMap<Camera*, SPtr<Camera>> _cameras;
        SPtr<RenderTarget> _mainRenderTarget;
        HEvent _mainRTResizedConn;

        ComponentState _componentState = ComponentState::Running;
    };

    /** Provides easy access to the SceneManager. */
    TE_CORE_EXPORT SceneManager& gSceneManager();
}
