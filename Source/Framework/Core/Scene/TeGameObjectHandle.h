#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    template <typename T>
    class GameObjectHandle;

    /**	Contains instance data that is held by all GameObject handles. */
    struct GameObjectInstanceData
    {
        GameObjectInstanceData()
            : Object(nullptr), InstanceId(0)
        { }

        SPtr<GameObject> Object;
        UINT64 InstanceId;
    };

    typedef SPtr<GameObjectInstanceData> GameObjectInstanceDataPtr;

    /**	Internal data shared between GameObject handles. */
    struct TE_CORE_EXPORT GameObjectHandleData
    {
        GameObjectHandleData() = default;

        GameObjectHandleData(SPtr<GameObjectInstanceData> ptr)
            : Ptr(std::move(ptr))
        { }

        SPtr<GameObjectInstanceData> Ptr;
    };

    /**
     * A handle that can point to various types of game objects. It primarily keeps track if the object is still alive,
     * so anything still referencing it doesn't accidentally use it.
     * 			
     * @note	
     * This class exists because references between game objects should be quite loose. For example one game object should
     * be able to reference another one without the other one knowing. But if that is the case I also need to handle the
     * case when the other object we're referencing has been deleted, and that is the main purpose of this class.	
     */
    class TE_CORE_EXPORT GameObjectHandleBase
    {
    public:
        GameObjectHandleBase()
            : _data(te_shared_ptr_new<GameObjectHandleData>(nullptr))
        { }

        /**
         * Returns true if the object the handle is pointing to has been destroyed.
         *
         * @param[in] checkQueued	Game objects can be queued for destruction but not actually destroyed yet, and still
         *							accessible. If this is false this method will return true only if the object is
         *							completely inaccessible (fully destroyed). If this is true this method will return true
         *							if object is completely inaccessible or if it is just queued for destruction.
         */
        bool IsDestroyed(bool checkQueued = false) const;

        /**	Returns the instance ID of the object the handle is referencing. */
        UINT64 GetInstanceId() const { return _data->Ptr != nullptr ? _data->Ptr->InstanceId : 0; }

        /**
         * Returns pointer to the referenced GameObject.
         *
         * @note	Throws exception if the GameObject was destroyed.
         */
        GameObject* Get() const
        {
            ThrowIfDestroyed();
            return _data->Ptr->Object.get();
        }

        /**
         * Returns a smart pointer to the referenced GameObject.
         *
         * @note	Throws exception if the GameObject was destroyed.
         */
        SPtr<GameObject> GetInternalPtr() const
        {
            ThrowIfDestroyed();
            return _data->Ptr->Object;
        }

        /**
         * Returns pointer to the referenced GameObject.
         *
         * @note	Throws exception if the GameObject was destroyed.
         */
        GameObject* operator->() const { return Get(); }

        /**
         * Returns reference to the referenced GameObject.
         *
         * @note	Throws exception if the GameObject was destroyed.
         */
        GameObject& operator*() const { return *Get(); }

    public:
        /** Returns internal handle data. */
        const SPtr<GameObjectHandleData>& GetHandleData() const { return _data; }

        /** Returns true if handled data is empty */
        bool Empty() const { return _data->Ptr == nullptr; }

        /**	Changes the GameObject instance the handle is pointing to. */
        void SetHandleData(const SPtr<GameObject>& object);

        /**	@copydoc SetHandleData */
        void SetHandleData(const GameObject& object);

        /**	@copydoc SetHandleData */
        void SetHandleData(const GameObject* object);

    protected:
        friend class GameObjectManager;

        /** Resolves a handle to a proper GameObject in case it was created uninitialized. */
        void Resolve(const GameObjectHandleBase& object) { _data->Ptr = object._data->Ptr; }

        template<class _Ty1, class _Ty2>
        friend bool operator==(const GameObjectHandle<_Ty1>& _Left, const GameObjectHandle<_Ty2>& _Right);

        GameObjectHandleBase(const SPtr<GameObject>& ptr);

        GameObjectHandleBase(const GameObject* ptr);

        GameObjectHandleBase(SPtr<GameObjectHandleData> data)
            : _data(std::move(data))
        { }

        GameObjectHandleBase(std::nullptr_t)
            : _data(te_shared_ptr_new<GameObjectHandleData>(nullptr))
        { }

        /**	Throws an exception if the referenced GameObject has been destroyed. */
        void ThrowIfDestroyed() const;

        /**	Invalidates the handle signifying the referenced object was destroyed. */
        void Destroy()
        {
            // It's important not to clear _data->Ptr as some code might rely
            // on it. (for example for restoring lost handles)

            if (_data->Ptr != nullptr)
                _data->Ptr->Object = nullptr;
        }

        SPtr<GameObjectHandleData> _data;
    };

    /**
     * @copydoc	GameObjectHandleBase
     *
     * @note	It is important this class contains no data since we often value cast it to its base.
     */
    template <typename T>
    class GameObjectHandle : public GameObjectHandleBase
    {
    public:
        /**	Constructs a new empty handle. */
        GameObjectHandle()
            : GameObjectHandleBase()
        {
            _data = te_shared_ptr_new<GameObjectHandleData>();
        }

        /**	Copy constructor from another handle of the same type. */
        GameObjectHandle(const GameObjectHandle<T>& ptr) = default;

        /**	Move constructor from another handle of the same type. */
        GameObjectHandle(GameObjectHandle<T> && ptr) = default;

        /**	Copy constructor from a shared ptr. */
        GameObjectHandle(const SPtr<T>& ptr)
            : GameObjectHandleBase(ptr)
        { }

        /**	Copy constructor from a ptr. */
        GameObjectHandle(const T* ptr)
            : GameObjectHandleBase(ptr)
        { }

        /**	Copy constructor from a reference. */
        GameObjectHandle(const T& ptr)
            : GameObjectHandleBase(&ptr)
        { }

        /**	Invalidates the handle. */
        GameObjectHandle<T>& operator=(std::nullptr_t ptr)
        {
            _data = te_shared_ptr_new<GameObjectHandleData>();
            return *this;
        }

        /** Created handle from a shared ptr */
        GameObjectHandle<T>& operator=(const SPtr<T>& ptr)
        {
            SetHandleData(ptr);
            return *this;
        }

        /** Copy assignment */
        GameObjectHandle<T>& operator=(const GameObjectHandle<T> & other) = default;

        /** Move assignment */
        GameObjectHandle<T>& operator=(GameObjectHandle<T> && other) = default;

        /**
         * Returns a pointer to the referenced GameObject.
         *
         * @note	Throws exception if the GameObject was destroyed.
         */
        T* Get() const
        {
            ThrowIfDestroyed();
            return reinterpret_cast<T*>(_data->Ptr->Object.get());
        }

        /**
         * Returns a smart pointer to the referenced GameObject.
         *
         * @note	Throws exception if the GameObject was destroyed.
         */
        SPtr<T> GetInternalPtr() const
        {
            ThrowIfDestroyed();
            return std::static_pointer_cast<T>(_data->Ptr->Object);
        }

        /**
         * Returns pointer to the referenced GameObject.
         *
         * @note	Throws exception if the GameObject was destroyed.
         */
        T* operator->() const { return Get(); }

        /**
         * Returns reference to the referenced GameObject.
         *
         * @note	Throws exception if the GameObject was destroyed.
         */
        T& operator*() const { return *Get(); }

        /** Converts a handle into a weak handle. */
        GameObjectHandle<T> GetNewHandleFromExisting() const
        {
            GameObjectHandle<T> handle;
            handle.SetHandleData(this->GetInternalPtr());

            return handle;
        }

    protected:
        template<class _Ty1, class _Ty2>
        friend GameObjectHandle<_Ty1> static_object_cast(const GameObjectHandle<_Ty2>& other);

        template<class _Ty1>
        friend GameObjectHandle<_Ty1> static_object_cast(const GameObjectHandleBase& other);

        GameObjectHandle(SPtr<GameObjectHandleData> data)
            :GameObjectHandleBase(std::move(data))
        { }

    public:
        template<class _Ty>
        struct Bool_struct
        {
            int _Member;
        };

        /**
         * Allows direct conversion of handle to bool.
         *
         * @note
         * This is needed because we can't directly convert to bool since then we can assign pointer to bool and that's
         * weird.
         */
        operator int Bool_struct<T>::* () const
        {
            return (((_data != nullptr) && (_data->Ptr != nullptr) && (_data->Ptr->Object != nullptr)) ? &Bool_struct<T>::_Member : 0);
        }
    };

    /**	Casts one GameObject handle type to another. */
    template<class _Ty1, class _Ty2>
    GameObjectHandle<_Ty1> static_object_cast(const GameObjectHandle<_Ty2>& other)
    {
        return GameObjectHandle<_Ty1>(other.GetHandleData());
    }

    /**	Casts a generic GameObject handle to a specific one . */
    template<class T>
    GameObjectHandle<T> static_object_cast(const GameObjectHandleBase& other)
    {
        return GameObjectHandle<T>(other.GetHandleData());
    }

    /**	Compares if two handles point to the same GameObject. */
    template<class _Ty1, class _Ty2>
    bool operator==(const GameObjectHandle<_Ty1>& _Left, const GameObjectHandle<_Ty2>& _Right)
    {
        return (_Left._data == nullptr && _Right._data == nullptr) ||
            (_Left._data != nullptr && _Right._data != nullptr && _Left.GetInstanceId() == _Right.GetInstanceId());
    }

    /**	Compares if two handles point to different GameObject%s. */
    template<class _Ty1, class _Ty2>
    bool operator!=(const GameObjectHandle<_Ty1>& _Left, const GameObjectHandle<_Ty2>& _Right)
    {
        return (!(_Left == _Right));
    }
}
