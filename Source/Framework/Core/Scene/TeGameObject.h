#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    /** Flags used for notifying child scene object and components when a transform has been changed. */
    enum TransformChangedFlags
    {
        TCF_None = 0x00, /**< Component will not be notified about any events relating to the transform. */
        TCF_Transform = 0x01, /**< Component will be notified when the its position, rotation or scale has changed. */
        TCF_Parent = 0x02, /**< Component will be notified when its parent changes. */
        TCF_Mobility = 0x04 /**< Component will be notified when mobility state changes. */
    };

    /**
     * Type of object that can be referenced by a GameObject handle. Each object has an unique ID and is registered with
     * the GameObjectManager.
     */
    class TE_CORE_EXPORT GameObject
    {
    public:
        GameObject() = default;
        virtual ~GameObject() = default;

        /**	Returns the unique instance ID of the GameObject. */
        UINT64 GetInstanceId() const { return _instanceData->InstanceId; }

        /**	Globally unique identifier of the game object that persists scene save/load. */
        const UUID& GetUUID() const { return _UUID; }

        /**	Gets the name of the object. */
        const String& GetName() const { return _name; }

        /**	Sets the name of the object. */
        void SetName(const String& name) { _name = name; }

    public:
        /**
         * Marks the object as destroyed. Generally this means the object has been queued for destruction but it hasn't
         * occurred yet.
         */
        void _setIsDestroyed() { _isDestroyed = true; }

        /**	Checks if the object has been destroyed. */
        bool _getIsDestroyed() const { return _isDestroyed; }

        /** @copydoc GetUUID */
        void _setUUID(const UUID& uuid) { _UUID = uuid; }

        /**
         * Replaces the instance data with another objects instance data. This object will basically become the original
         * owner of the provided instance data as far as all game object handles referencing it are concerned.
         *
         * @note
         * No alive objects should ever be sharing the same instance data. This can be used for restoring dead handles.
         */
        virtual void _setInstanceData(GameObjectInstanceDataPtr& other);

        /** Returns instance data that identifies this GameObject and is used for referencing by game object handles. */
        virtual GameObjectInstanceDataPtr _getInstanceData() const { return _instanceData; }

    protected:
        friend class GameObjectHandleBase;
        friend class GameObjectManager;

        /**	Initializes the GameObject after construction. */
        void Initialize(const SPtr<GameObject>& object, UINT64 instanceId);

        /**
         * Destroys this object.
         *
         * @param[in]	handle		Game object handle to this object.
         * @param[in]	immediate	If true, the object will be deallocated and become unusable right away. Otherwise the
         *							deallocation will be delayed to the end of frame (preferred method).
         */
        virtual void DestroyInternal(GameObjectHandleBase& handle, bool immediate = false) = 0;

    protected:
        String _name;
        UUID _UUID;

    private:
        GameObjectInstanceDataPtr _instanceData;
        bool _isDestroyed = false;
    };
}