#pragma once

#include "TeCorePrerequisites.h"
#include "TeCoreObjectManager.h"

namespace te
{
    /** Core objects provides a standardized way to initialize/destroy objects. */
    class TE_CORE_EXPORT CoreObject
    {
    protected:
        /** Values that represent current state of the core object */
        enum Flags
        {
            CGO_NONE = 0x00,  /**< Object has not yet been initialized nor destroyed. */
            CGO_DESTROYED = 0x01, /**< Object has been destroyed and shouldn't be used. */
            CGO_INITIALIZED = 0x02 /**< Object's initialize() method has been called. */
        };

    public:
        /** Schedules the object to be destroyed, and then deleted. */
        template<class T>
        static void _delete(CoreObject* obj)
        {
            if (!obj->IsDestroyed())
            {
                obj->Destroy();
            }

            te_delete<T>((T*)obj);
        }

        /**
         * Initializes all the internal resources of this object. Must be called right after construction. Generally you
         * should call this from a factory method to avoid the issue where user forgets to call it.
         */
        virtual void Initialize();

        /** Frees all the data held by this object. */
        virtual void Destroy();

        /** Returns true if the object has been initialized. Non-initialized object should not be used. */
        bool IsInitialized() const { return (_flags & CGO_INITIALIZED) != 0; }

        /** Returns true if the object has been destroyed. Destroyed object should not be used. */
        bool IsDestroyed() const { return (_flags & CGO_DESTROYED) != 0; }

        /** Returns an unique identifier for this object. */
        UINT64 GetInternalID() const { return _internalID; }

        /** Returns a shared_ptr version of "this" pointer. */
        SPtr<CoreObject> GetThisPtr() const { return _this.lock(); };

        /**
         * Sets a shared this pointer to this object. This must be called immediately after construction, but before
         * initialize().
         *
         * @note This should be called by the factory creation methods so user doesn't have to call it manually.
         */
        void SetThisPtr(SPtr<CoreObject> ptrThis);

        /**
         * Marks the core data as dirty. 
         *
         * @param[in]	flags	(optional)	Flags in case you want to signal that only part of the internal data is dirty.
         */
        void MarkCoreDirty(UINT32 flags = 0xFFFFFFFF);

        /**
         * Marks the provided dirty flag
         *
         * @param[in]	flags	(optional)	Flags in case you want to signal that only part of the internal data is dirty.
         */
        void RemoveDirtyFlag(UINT32 flags);

        /** Marks the core data as clean. */
        void MarkCoreClean() { _coreDirtyFlags = 0; }

        /**
         * Checks is the core dirty flag set. This is used by external systems to know when internal data has changed and
         * core thread potentially needs to be notified.
         */
        bool IsCoreDirty() const { return _coreDirtyFlags != 0; }

        /**
         * Returns the exact value of the internal flag that signals whether an object needs to be synced with the core thread.
         */
        UINT32 GetCoreDirtyFlags() const { return _coreDirtyFlags; }

    protected:
        /** Constructs a new core object. */
        CoreObject();
        virtual ~CoreObject();

        void SetIsDestroyed(bool destroyed) { _flags = destroyed ? _flags | CGO_DESTROYED : _flags & ~CGO_DESTROYED; }

    private:
        volatile UINT8 _flags;
        UINT32 _coreDirtyFlags;
        UINT64 _internalID; // ID == 0 is not a valid ID
        WPtr<CoreObject> _this;
    };

    /**
     * Creates a core object shared pointer using a previously constructed object.
     *
     * @note
     * All core thread object shared pointers must be created using this method or its overloads and you should not create
     * them manually.
     */
    template<class Type>
    SPtr<Type> te_core_ptr(Type* data)
    {
        return SPtr<Type>(data, &CoreObject::_delete<Type>);
    }

    /**
     * Creates a new core object using the specified allocators and returns a shared pointer to it.
     *
     * @note
     * All core thread object shared pointers must be created using this method or its overloads and you should not create
     * them manually.
     */
    template<class Type, class... Args>
    SPtr<Type> te_core_ptr_new(Args&&...args)
    {
        return SPtr<Type>(te_new<Type>(std::forward<Args>(args)...), &CoreObject::_delete<Type>);
    }
}
