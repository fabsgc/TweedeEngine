#pragma once

#include "TeCorePrerequisites.h"
#include "Scene/TeGameObject.h"
#include "Utility/TeEvent.h"

namespace te
{
    typedef UINT32 ComponentFlags;

    /**
     * Components represent primary logic elements in the scene. They are attached to scene objects.
     *
     * You should implement some or all of Update/OnCreated/OnInitialized/OnEnabled/OnDisabled/
     * OnTransformChanged/onDestroyed methods to implement the relevant component logic. Avoid putting logic in constructors
     * or destructors.
     **/
    class TE_CORE_EXPORT Component : public GameObject, public Serializable
    {
    public:
        /** Each component has a type which is used to recognize and cast components */
        static inline UINT32 GetComponentType()
        {
            return TypeID_Core::TID_Component;
        }

        /** Sometimes, you could have to change parent of a component */
        void SetSceneObject(HSceneObject& sceneObject);

        /**	Returns the SceneObject this Component is assigned to. */
        const HSceneObject& GetSceneObject() const { return _parent; }

        /** @copydoc sceneObject */
        const HSceneObject& SO() const { return GetSceneObject(); }

        /**	Returns a handle to this object. */
        const HComponent& GetHandle() const { return _thisHandle; }

        /** */
        virtual void Initialize();

        /** Called once per frame. Only called if the component is in Running state. */
        virtual void Update() { }

        /**
         * Calculates bounds of the visible contents represented by this component (for example a mesh for Renderable).
         *
         * @param[in]	bounds	Bounds of the contents in world space coordinates.
         * @return				True if the component has bounds with non-zero volume, otherwise false.
         */
        virtual bool CalculateBounds(Bounds& bounds);

        /** Checks if this and the provided component represent the same type. */
        virtual bool TypeEquals(const Component& other);

        /**
         * Removes the component from parent SceneObject and deletes it. All the references to this component will be
         * marked as destroyed and you will get an exception if you try to use them.
         *
         * @param[in]	immediate	If true the destruction will be performed immediately, otherwise it will be delayed
         *							until the end of the current frame (preferred option).
         */
        void Destroy(bool immediate = false);

    public:
        /**
         * Construct any resources the component needs before use. Called when the parent scene object is instantiated.
         * A non-instantiated component shouldn't be used
         */
        virtual void Instantiate() {}

        /** Sets new flags that determine when is OnTransformChanged called. */
        void SetNotifyFlags(UINT32 flags) { _notifyFlags = flags; }

        /** Gets the currently assigned notify flags. See SetNotifyFlags(). */
        UINT32 GetNotifyFlags() const { return _notifyFlags; }

        /** Enables or disabled a flag controlling component's behaviour. */
        void SetFlag(UINT32 flag, bool enabled) { if (enabled) _flags |= flag; else _flags &= ~flag; }

        /** Checks if the component has a certain flag enabled. */
        bool HasFlag(UINT32 flag) const { return _flags | flag; }

        /**
         * Returns true if the this is below the sceneObject given in paramter in the scene hierarchy
         *
         * @param[in]	sceneObject			SceneObject handle
         * @return		true if this is a descendant of the sceneObject given in paramter
         */
        bool IsDescendantOf(const HSceneObject& sceneObject);

        /**
         * Returns true if the this is a direct child of the SceneObject given in parameter
         *
         * @param[in]	sceneObject			SceneObject handle
         * @return		true if this is a direct child of the sceneObject given in paramter
         */
        bool IsChildOf(const HSceneObject& sceneObject);

        /** Notify that internal data (if exists) must be updated */
        virtual void MarkDirty() { }

        /** Call when a new component is created */
        static Event<void(const HComponent&)> OnComponentCreated;

        /** Call when a new component is initialized */
        static Event<void(const HComponent&)> OnComponentInitialized;

        /** Called when the component is destoyed */
        static Event<void(const HComponent&)> OnComponentDestroyed;

        /** Called when the component is enabled */
        static Event<void(const HComponent&)> OnComponentEnabled;

        /** Called when the component is disabled */
        static Event<void(const HComponent&)> OnComponentDisabled;

    protected:
        Component(HSceneObject parent, UINT32 type);
        virtual ~Component() = 0 { };

        /**
         * If you want to create a copy of a component, first use SceneObject::AddComponent() then, use this method to
         * properly copy all from "c" component
         */
        bool Clone(const HComponent& c, const String& suffix = "");

        /** @copydoc Component::Clone */
        bool Clone(const SPtr<Component>& c, const String& suffix = "");

        /** 
         * Called once when the component has been created. Called regardless of the state the component is in. 
         */
        virtual void OnCreated();

        /**
         * Called once when the component first leaves the Stopped state. This includes component creation if requirements
         * for leaving Stopped state are met, in which case it is called after onCreated.
         */
        virtual void OnInitialized();

        /**	
         * Called once just before the component is destroyed. Called regardless of the state the component is in. 
         */
        virtual void OnDestroyed();

        /**
         * Called every time a component switchs in Stopped state. This includes component creation if requirements
         * for leaving the Stopped state are met. When called during creation it is called after onInitialized.
         */
        virtual void OnDisabled();

        /**
         * Called every time a component leaves the Stopped state. This includes component creation if requirements
         * for leaving the Stopped state are met. When called during creation it is called after onInitialized.
         */
        virtual void OnEnabled();

        /**
         * Called when the component's parent scene object has changed.
         * Also only called if necessary notify flags are set via SetNotifyFlags().
         */
        virtual void OnTransformChanged(TransformChangedFlags flags) { }

        /** Checks whether the component wants to received the specified transform changed message. */
        bool SupportsNotify(TransformChangedFlags flags) const { return ( _notifyFlags & flags) != 0; }

        /** Sets an index that uniquely identifies a component with the SceneManager. */
        void SetSceneManagerId(UINT32 id) { _sceneManagerId = id; }

        /** Returns an index that unique identifies a component with the SceneManager. */
        UINT32 GetSceneManagerId() const { return _sceneManagerId; }

        /**
         * Destroys this component.
         *
         * @param[in]	handle		Game object handle this this object.
         * @param[in]	immediate	If true, the object will be deallocated and become unusable right away. Otherwise the
         *							deallocation will be delayed to the end of frame (preferred method).
         *
         * @note	Unlike destroy(), does not remove the component from its parent.
         */
        void DestroyInternal(GameObjectHandleBase & handle, bool immediate) override;

    private:
        /** We can't allow user to create a copy of a component without using engine API */
        Component(const Component& other) = delete;

    protected:
        friend class SceneManager;
        friend class SceneObject;

        HComponent _thisHandle;
        UINT32 _notifyFlags;
        ComponentFlags _flags;
        UINT32 _sceneManagerId;

        HSceneObject _parent;

    public:
        static UINT32 ComponentType;
        static const UINT32 AlwaysRun;
    };
}
