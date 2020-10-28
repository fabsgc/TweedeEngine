#pragma once

#include "TeCorePrerequisites.h"
#include "Math/TeMatrix4.h"
#include "Math/TeVector3.h"
#include "Math/TeQuaternion.h"
#include "Scene/TeGameObjectManager.h"
#include "Scene/TeGameObject.h"
#include "Scene/TeComponent.h"
#include "Scene/TeTransform.h"
#include "Serialization/TeSerializable.h"

#include <any>

namespace te
{
    class SceneInstance;

    /**
     * An object in the scene graph. It has a transform object that allows it to be positioned, scaled and rotated. It can
     * have other scene objects as children, and will have a scene object as a parent, in which case transform changes
     * to the parent are reflected to the child scene objects (children are relative to the parent).
     *
     * Each scene object can have one or multiple Components attached to it, where the components inherit the scene
     * object's transform, and receive updates about transform and hierarchy changes.
     */
    class TE_CORE_EXPORT SceneObject : public GameObject, public Serializable
    {
        /** Flags that signify which part of the SceneObject needs updating. */
        enum DirtyFlags
        {
            LocalTfrmDirty = 0x01,
            WorldTfrmDirty = 0x02
        };

        enum class ComponentSearchType
        {
            CoreType, Name, UUID
        };

        friend class SceneManager;

    public:
        ~SceneObject();

        /**
         * Creates a new SceneObject with the specified name. Object will be placed in the top of the scene hierarchy.
         *
         * @param[in]	name	Name of the scene object.
         * @param[in]	flags	Optional flags that control object behavior. See SceneObjectFlags.
         */
        static HSceneObject Create(const String& name, UINT32 flags = 0);

        /**
         * Destroys this object and any of its held components.
         *
         * @param[in]	immediate	If true, the object will be deallocated and become unusable right away. Otherwise the
         *							deallocation will be delayed to the end of frame (preferred method).
         */
        void Destroy(bool immediate = false);

        /**	Returns a handle to this object. */
        HSceneObject GetHandle() const { return _thisHandle; }

        /**	Checks if the scene object has a specific bit flag set. */
        bool HasFlag(UINT32 flag) const;

        /**
         * If you want to create a copy of a sceneObject, first use SceneObject::Create() then, use this method to
         * properly copy all from "so" sceneObject
         */
        void Clone(const HSceneObject so);

        /**
         * If you want to create a copy of a sceneObject, first use SceneObject::Create() then, use this method to
         * properly copy all from "so" sceneObject
         */
        void Clone(const SPtr<SceneObject>& so);

    private: // ***** INTERNAL ******
        /** @copydoc GameObject::_setInstanceData */
        void _setInstanceData(GameObjectInstanceDataPtr& other) override;

        /** Recursively enables the provided set of flags on this object and all children. */
        void _setFlags(UINT32 flags);

        /** Recursively disables the provided set of flags on this object and all children. */
        void _unsetFlags(UINT32 flags);

    private:
        SceneObject(const String& name, UINT32 flags);

        /** We can't allow user to create a copy of a scene object without using engine API */
        SceneObject(const SceneObject& other) = delete;

        /**
         * Creates a new SceneObject instance, registers it with the game object manager, creates and returns a handle to
         * the new object.
         *
         * @note
         * When creating objects with DontInstantiate flag it is the callers responsibility to manually destroy the object,
         * otherwise it will leak.
         */
        static HSceneObject CreateInternal(const String& name, UINT32 flags = 0);

        /**
         * Creates a new SceneObject instance from an existing pointer, registers it with the game object manager, creates
         * and returns a handle to the object.
         *
         * @param[in]	soPtr		Pointer to the scene object register and return a handle to.
         */
        static HSceneObject CreateInternal(const SPtr<SceneObject>& soPtr);

        /**
         * Destroys this object and any of its held components.
         *
         * @param[in]	handle		Game object handle to this object.
         * @param[in]	immediate	If true, the object will be deallocated and become unusable right away. Otherwise the
         *							deallocation will be delayed to the end of frame (preferred method).
         * @note	Unlike destroy(), does not remove the object from its parent.
         */
        void DestroyInternal(GameObjectHandleBase& handle, bool immediate = false) override;

    public:
        /** Gets the transform object representing object's position/rotation/scale in world space. */
        const Transform& GetTransform() const;

        /** Gets the transform object representing object's position/rotation/scale relative to its parent. */
        const Transform& GetLocalTransform() const { return _localTfrm; }

        /** Gets the transform object representing object's position/rotation/scale relative to its parent. */
        void SetLocalTransform(Transform& tfrm) { _localTfrm = tfrm; }

        /**	Sets the local position of the object. */
        void SetPosition(const Vector3& position);

        /**	Sets the world position of the object. */
        void SetWorldPosition(const Vector3& position);

        /**	Sets the local rotation of the object. */
        void SetRotation(const Quaternion& rotation);

        /**	Sets the world rotation of the object. */
        void SetWorldRotation(const Quaternion& rotation);

        /**	Sets the local scale of the object. */
        void SetScale(const Vector3& scale);

        /**
         * Sets the world scale of the object.
         * @note	This will not work properly if this object or any of its parents have non-affine transform matrices.
         */
        void SetWorldScale(const Vector3& scale);

        /**
         * Orients the object so it is looking at the provided @p location (world space) where @p up is used for
         * determining the location of the object's Y axis.
         */
        virtual void LookAt(const Vector3& location, const Vector3& up = Vector3::UNIT_Y);

        /**
         * Gets the objects world transform matrix.
         * @note	Performance warning: This might involve updating the transforms if the transform is dirty.
         */
        const Matrix4& GetWorldMatrix() const;

        /**
         * Gets the objects inverse world transform matrix.
         * @note	Performance warning: This might involve updating the transforms if the transform is dirty.
         */
        Matrix4 GetInvWorldMatrix() const;

        /** Gets the objects local transform matrix. */
        const Matrix4& GetLocalMatrix() const;

        /**	Moves the object's position by the vector offset provided along world axes. */
        void Move(const Vector3& vec);

        /**	Moves the object's position by the vector offset provided along it's own axes (relative to orientation). */
        void MoveRelative(const Vector3& vec);

        /**
         * Rotates the game object so it's forward axis faces the provided direction.
         * @param[in]	forwardDir	The forward direction to face, in world space.
         * @note	Local forward axis is considered to be negative Z.
         */
        void SetForward(const Vector3& forwardDir);

        /**	Rotate the object around an arbitrary axis. */
        void Rotate(const Vector3& axis, const Radian& angle);

        /**	Rotate the object around an arbitrary point using an axis and an angle. */
        void RotateAround(const Vector3& center, const Vector3& axis, const Radian& angle);

        /**	Rotate the object around an arbitrary point using a quaternion. */
        void RotateAround(const Vector3& center, const Quaternion& rotation);

        /**	Rotate the object around an arbitrary axis using a Quaternion. */
        void Rotate(const Quaternion& q);

        /**
         * Rotates around local Z axis.
         * @param[in]	angle	Angle to rotate by.
         */
        void Roll(const Radian& angle);

        /**
         * Rotates around Y axis.
         * @param[in]	angle	Angle to rotate by.
         */
        void Yaw(const Radian& angle);

        /**
         * Rotates around X axis
         * @param[in]	angle	Angle to rotate by.
         */
        void Pitch(const Radian& angle);

        /**
         * Forces any dirty transform matrices on this object to be updated.
         *
         * @note
         * Normally this is done internally when retrieving a transform, but sometimes it is useful to update transforms
         * manually.
         */
        void UpdateTransformsIfDirty();

        /**
         * Returns a hash value that changes whenever a scene objects transform gets updated. It allows you to detect
         * changes with the local or world transforms without directly comparing their values with some older state.
         */
        UINT32 GetTransformHash() const { return _dirtyHash; }

        /**
         * Removes the component from this object, and deallocates it.
         *
         * @param[in]	component	The component to destroy.
         * @param[in]	immediate	If true, the component will be deallocated and become unusable right away. Otherwise
         *							the deallocation will be delayed to the end of frame (preferred method).
         */
        void DestroyComponent(const HComponent component, bool immediate = false);

        /**
         * Removes the component from this object, and deallocates it.
         *
         * @param[in]	component	The component to destroy.
         * @param[in]	immediate	If true, the component will be deallocated and become unusable right away. Otherwise
         *							the deallocation will be delayed to the end of frame (preferred method).
         */
        void DestroyComponent(Component* component, bool immediate = false);

    private:
        /**
         * Notifies components and child scene object that a transform has been changed.
         * @param	flags		Specifies in what way was the transform changed.
         */
        void NotifyTransformChanged(TransformChangedFlags flags) const;

        /** Updates the local transform. Normally just reconstructs the transform matrix from the position/rotation/scale. */
        void UpdateLocalTfrm() const;

        /**
         * Updates the world transform. Reconstructs the local transform matrix and multiplies it with any parent transforms.
         * @note	If parent transforms are dirty they will be updated.
         */
        void UpdateWorldTfrm() const;

        /**	Checks if cached local transform needs updating. */
        bool IsCachedLocalTfrmUpToDate() const { return (_dirtyFlags & DirtyFlags::LocalTfrmDirty) == 0; }

        /**	Checks if cached world transform needs updating. */
        bool IsCachedWorldTfrmUpToDate() const { return (_dirtyFlags & DirtyFlags::WorldTfrmDirty) == 0; }

    public: // ***** HIERARCHY ******
        /**
         * Changes the parent of this object. Also removes the object from the current parent, and assigns it to the new
         * parent.
         *
         * @param[in]	parent				New parent.
         * @param[in]	keepWorldTransform	Determines should the current transform be maintained even after the parent is
         *									changed (this means the local transform will be modified accordingly).
         */
        void SetParent(const HSceneObject& parent, bool keepWorldTransform = true);

        /**
         * Gets the parent of this object.
         * @return	Parent object, or nullptr if this SceneObject is at root level.
         */
        HSceneObject GetParent() const { return _parent; }

        /**
         * Gets a child of this item.
         *
         * @param[in]	idx	The zero based index of the child.
         * @return		SceneObject of the child.
         */
        HSceneObject GetChild(UINT32 idx) const;

        /**	Returns all children on this object. */
        const Vector<HSceneObject>& GetChildren() const { return _children; }

        /**
         * Find the index of the specified child. Don't persist this value as it may change whenever you add/remove children.
         *
         * @param[in]	child	The child to look for.
         * @return				The zero-based index of the found child, or -1 if no match was found.
         */
        int IndexOfChild(const HSceneObject& child) const;

        /**	Gets the number of all child GameObjects. */
        UINT32 GetNumChildren() const { return (UINT32)_children.size(); }

        /** Returns the scene this object is part of. Can be null if scene object hasn't been instantiated. */
        const SPtr<SceneInstance>& GetScene() const;

        /**
         * Sets the mobility of a scene object. This is used primarily as a performance hint to engine systems. Objects
         * with more restricted mobility will result in higher performance. Some mobility constraints will be enforced by
         * the engine itself, while for others the caller must be sure not to break the promise he made when mobility was
         * set. By default scene object's mobility is unrestricted.
         */
        void SetMobility(ObjectMobility mobility);

        /** Gets the mobility setting for this scene object. See setMobility(); */
        ObjectMobility GetMobility() const { return _mobility; }

    private: // ***** HIERARCHY ******
        /**
         * Internal version of setParent() that allows you to set a null parent.
         *
         * @param[in]	parent				New parent.
         * @param[in]	keepWorldTransform	Determines should the current transform be maintained even after the parent is
         *									changed (this means the local transform will be modified accordingly).
         */
        void _setParent(const HSceneObject& parent, bool keepWorldTransform = true);

        /** Changes the owning scene of the scene object and all children. */
        void SetScene(const SPtr<SceneInstance>& scene);

        /**
         * Adds a child to the child array. This method doesn't check for null or duplicate values.
         *
         * @param[in]	object	New child.
         */
        void AddChild(const HSceneObject& object);

        /**
         * Removes the child from the object.
         *
         * @param[in]	object	Child to remove.
         */
        void RemoveChild(const HSceneObject& object);

    public: // ***** COMPONENT ******
        /** Constructs a new component of the specified type and adds it to the internal component list. */
        template<class T, class... Args>
        GameObjectHandle<T> AddComponent(Args&&... args)
        {
            static_assert((std::is_base_of<te::Component, T>::value),
                "Specified type is not a valid Component.");

            SPtr<T> gameObject(new (te_allocate<T>()) T(_thisHandle,
                std::forward<Args>(args)...), &te_delete<T>);

            const HComponent newComponent =
                static_object_cast<Component>(GameObjectManager::Instance().RegisterObject(gameObject));

            AddAndInitializeComponent(newComponent);
            return static_object_cast<T>(newComponent);
        }

        /**
         * Checks if the current object contains the specified component or components derived from the provided type.
         *
         * @tparam	typename T	Type of the component.
         * @return				True if component exists on the object.
         *
         * @note	Don't call this too often as it is relatively slow.
         */
        bool HasComponent(UINT32 type)
        {
            for (auto entry : _components)
            {
                if (entry->GetCoreType() == type)
                    return true;
            }

            return false;
        }

        /**	Creates an empty component with the default constructor. Primarily used for RTTI purposes. */
        template <typename T>
        static SPtr<T> CreateEmptyComponent()
        {
            static_assert((std::is_base_of<te::Component, T>::value), "Specified type is not a valid Component.");

            T* rawPtr = new (te_allocate<T>()) T();
            SPtr<T> gameObject(rawPtr, &te_delete<T>);

            return gameObject;
        }

    public: // ***** COMPONENT AND SCENEOBJECT SEARCH ******

        /**	Returns all components on this object. */
        const Vector<HComponent>& GetComponents() const { return _components; }

        /**
         * Searches for a component with the specified type accross all direct children and returns the first one it finds.
         *
         * @param[in]	type				RTTI information for the type.
         * @param[in]	searchInChildren	if true, will also search in children SceneObject
         * @return		Component if found, nullptr otherwise.
         */
        HComponent GetComponent(UINT32 type, bool searchInChildren = false) const;

        /**
         * Searches for components with the specified type accross all direct children and returns all that match.
         *
         * @param[in]	type				RTTI information for the type.
         * @param[in]	searchInChildren	if true, will also search in children SceneObject
         * @return		Vector of Components
         */
        Vector<HComponent> GetComponents(UINT32 type, bool searchInChildren = false) const;

        /**
         * Searches for a component with the specified name accross all direct children and returns the first one it finds.
         *
         * @param[in]	name				name of the component
         * @param[in]	searchInChildren	if true, will also search in children SceneObject
         * @return		Component if found, nullptr otherwise.
         */
        HComponent GetComponent(const String& name, bool searchInChildren = false) const;

        /**
         * Searches for components with the specified type accross all direct children and returns all that match.
         *
         * @param[in]	name				name of the component
         * @param[in]	searchInChildren	if true, will also search in children SceneObject
         * @return		Vector ofComponents
         */
        Vector<HComponent> GetComponents(const String& name, bool searchInChildren = false) const;

        /**
         * Searches for components with the specified uuid accross all direct children and returns all that match.
         *
         * @param[in]	uuid				uuid of the component
         * @param[in]	searchInChildren	if true, will also search in children SceneObject
         * @return		Vector of Components
         */
        Vector<HComponent> GetComponents(const UUID& uuid, bool searchInChildren = false) const;

        /**
         * Searches for a component with the specified uuid accross all direct children and returns the first one it finds.
         *
         * @param[in]	uuid				uuid of the component
         * @param[in]	searchInChildren	if true, will also search in children SceneObject
         * @return		Component if found, nullptr otherwise.
         */
        HComponent GetComponent(const UUID& uuid, bool searchInChildren = false) const;

        /**
         * Searches for a scene object with the specified name accross all direct children and returns the first one it finds.
         *
         * @param[in]	name				name of the scene object
         * @return		SceneObject if found, nullptr otherwise.
         */
        HSceneObject GetSceneObject(const String& name, bool searchInChildren = false) const;

        /**
         * Searches for a scene object with the specified name accross all direct children and returns all that match.
         *
         * @param[in]	name				name of the scene object
         * @return		Vector of SceneObjects
         */
        Vector<HSceneObject> GetSceneObjects(const String& name, bool searchInChildren = false) const;

        /**
         * Searches for a scene object with the specified name accross all direct children and returns the first one it finds.
         *
         * @param[in]	uuid				uuid of the scene object
         * @return		SceneObject if found, nullptr otherwise.
         */
        HSceneObject GetSceneObject(const UUID& uuid, bool searchInChildren = false) const;

        /**
         * Searches for a scene object with the specified name accross all direct children and returns all that match.
         *
         * @param[in]	uuid				uuid of the scene object
         * @return		Vector of SceneObjects
         */
        Vector<HSceneObject> GetSceneObjects(const UUID& uuid, bool searchInChildren = false) const;

        /**
         * Returns true if the current sceneObject is below the sceneObject given in parameter in the scene hierarchy
         * 
         * @param[in]	sceneObject			scene object handle
         * @return		true if this is a descendant of the sceneObject given in paramter
         */
        bool IsDescendantOf(const HSceneObject& sceneObject);

    private:
        /**
         * Searches for components with the specified type accross all direct children of the current sceneObject and returns the first one it finds.
         *
         * @param[in]	currentSO			current sceneObject of the search
         * @param[in]	criteria			search criteria
         * @param[in]	searchType			type of search we want to do (by type, name or uuid)
         * @return		Component if found, nullptr otherwise.
         */
        HComponent _getComponentInternal(const HSceneObject& currentSO, std::any criteria, ComponentSearchType searchType, bool searchInChildren = false) const;

        /**
         * Searches for components with the specified type accross all direct children of the current sceneObject and returns all that match
         *
         * @param[in]	currentSO			current sceneObject of the search
         * @param[in]	type				RTTI information for the type.
         * @param[in]	components			vector of components where we can put all found components
         * @param[in]	searchType			type of search we want to do (by type, name or uuid)
         * @return		void
         */
        void _getComponentsInternal(const HSceneObject& currentSO, std::any criteria, Vector<HComponent>& components, ComponentSearchType searchType, bool searchInChildren = false) const;

        /**
         * Searches for scene object with the specified type accross all direct children of the current sceneObject and returns the first one it finds.
         *
         * @param[in]	currentSO			current sceneObject of the search
         * @param[in]	criteria			search criteria
         * @param[in]	searchType			type of search we want to do (by type, name or uuid)
         * @return		SceneObject if found, nullptr otherwise.
         */
        HSceneObject _getSceneObjectInternal(const HSceneObject& currentSO, std::any criteria, ComponentSearchType searchType, bool searchInChildren = false) const;

        /**
         * Searches for components with the specified type accross all direct children of the current sceneObject returns all that match
         *
         * @param[in]	currentSO			current sceneObject of the search
         * @param[in]	type				RTTI information for the type.
         * @param[in]	sceneObjects		vector of SceneObjects where we can put all found SceneObjects
         * @param[in]	searchType			type of search we want to do (by type, name or uuid)
         * @return		void
         */
        void _getSceneObjectsInternal(const HSceneObject& currentSO, std::any criteria, Vector<HSceneObject>& sceneObjects, ComponentSearchType searchType, bool searchInChildren = false) const;

    private:
        /**	Adds the component to the internal component array. */
        void AddComponentInternal(const SPtr<Component>& component);

        /**	Adds the component to the internal component array, and initializes it. */
        void AddAndInitializeComponent(const HComponent& component);

        /**	Adds the component to the internal component array, and initializes it. */
        void AddAndInitializeComponent(const SPtr<Component>& component);

    private:
        Transform _localTfrm;
        mutable Transform _worldTfrm;

        mutable Matrix4 _cachedLocalTfrm = Matrix4::IDENTITY;
        mutable Matrix4 _cachedWorldTfrm = Matrix4::IDENTITY;

        mutable UINT32 _dirtyFlags = 0xFFFFFFFF;
        mutable UINT32 _dirtyHash = 0;

        HSceneObject _thisHandle;
        UINT32 _flags;

        SPtr<SceneInstance> _parentScene;
        HSceneObject _parent;
        Vector<HSceneObject> _children;
        ObjectMobility _mobility = ObjectMobility::Movable;

        Vector<HComponent> _components;
    };
}
