#pragma once

#include "Utility/TeUUID.h"

namespace te
{
    /** Data that is shared between all resource handles. */
    struct TE_CORE_EXPORT ResourceHandleData
    {
        SPtr<Resource> data;
        te::UUID uuid;
    };

    /** Manager that handles resource loading. */
    class TE_CORE_EXPORT ResourceHandleBase
    {
    public:
        /**
         * Checks if the resource is loaded. Until resource is loaded this handle is invalid and you may not get the
         * internal resource from it.
         */
        bool IsLoaded() const;

        /** Releases an internal reference to this resource held by the resources system, if there is one. */
        void Release();

        /** Returns the UUID of the resource the handle is referring to. */
        const te::UUID& GetUUID() const { return _handleData != nullptr ? _handleData->uuid : te::UUID::EMPTY; }

        /** Change the UUID of the current handled resource. */
        void SetUUID(const te::UUID& uuid) { if (!uuid.Empty() && _handleData != nullptr) { _handleData->uuid = uuid; } }

        /** Gets the handle data. For internal use only. */
        const SPtr<ResourceHandleData>& GetHandleData() const { return _handleData; }

    private:
        friend class ResourceManager;

    protected:
        /** Destroys the resource the handle is pointing to. */
        void Destroy();

        /**
         * Sets the created flag to true and assigns the resource pointer. Called by the constructors, or if you
         * constructed just using a UUID, then you need to call this manually before you can access the resource from
         * this handle.
         */
        void SetHandleData(const SPtr<Resource>& resource, const UUID& uuid);

        /**
         * Clears the created flag and the resource pointer, making the handle invalid until the resource is loaded again
         * and assigned through setHandleData().
         */
        void ClearHandleData();

    protected:
        SPtr<ResourceHandleData> _handleData;
    };

    template <typename T>
    class TResourceHandle : public ResourceHandleBase
    {
    public:
        TResourceHandle() = default;
        virtual ~TResourceHandle() {}

        /** Copy constructor. */
        TResourceHandle(const TResourceHandle& other)
        {
            this->_handleData = other.GetHandleData();
        }

        /** Move constructor. */
        TResourceHandle(TResourceHandle&& other) = default;

        /** Converts a specific handle to generic Resource handle. */
        operator TResourceHandle<Resource>() const
        {
            TResourceHandle<Resource> handle;
            handle.SetHandleData(this->GetHandleData());

            return handle;
        }

        /** Returns internal resource pointer. */
        T* operator->() const { return Get(); }

        /** Returns internal resource pointer and dereferences it. */
        T& operator*() const { return *Get(); }

        /** Clears the handle making it invalid and releases any references held to the resource. */
        TResourceHandle<T>& operator=(std::nullptr_t ptr)
        {
            this->_handleData = nullptr;
            return *this;
        }

        /** Copy assignment. */
        TResourceHandle<T>& operator=(const TResourceHandle<T>& rhs)
        {
            SetHandleData(rhs.GetHandleData());
            return *this;
        }

        /** Move assignment. */
        TResourceHandle& operator=(TResourceHandle&& other)
        {
            if(this == &other)
            {
                return *this;
            }

            this->_handleData = std::exchange(other._handleData, nullptr);
            return *this;
        }

        /** Returns internal resource pointer and dereferences it. */
        T* Get() const
        {
            return reinterpret_cast<T*>(this->_handleData->data.get());
        }

        /** Returns the internal shared pointer to the resource. */
        SPtr<T> GetInternalPtr() const
        {
            return std::static_pointer_cast<T>(this->_handleData->data);
        }

        /** Converts a handle into a weak handle. */
        TResourceHandle<T> GetNewHandleFromExisting() const
        {
            TResourceHandle<T> handle;
            handle.SetHandleData(this->GetHandleData());

            return handle;
        }

        /** Create a handle from a valid resource pointer */
        static TResourceHandle<T> GetNewHandleFromExisting(T* ptr)
        {
            if (ptr)
            {
                return TResourceHandle<T>(ptr, ptr->GetUUID());
            }

            return TResourceHandle<T>();
        }

    protected:
        friend ResourceManager;
        template<class _T>
        friend class TResourceHandle;
        template<class _Ty1, class _Ty2>
        friend TResourceHandle<_Ty1> static_resource_cast(const TResourceHandle<_Ty2>& other);

        /**
         * Constructs a new valid handle for the provided resource with the provided UUID.
         * @note Handle will take ownership of the provided resource pointer, so make sure you don't delete it elsewhere.
         */
        explicit TResourceHandle(T* ptr, const te::UUID& uuid)
            : ResourceHandleBase()
        {
            this->_handleData = te_shared_ptr_new<ResourceHandleData>();
            this->SetHandleData(SPtr<Resource>(ptr), uuid);
        }

        /**
         * Constructs an invalid handle with the specified UUID. You must call setHandleData() with the actual resource
         * pointer to make the handle valid.
         */
        TResourceHandle(const te::UUID& uuid)
        {
            this->_handleData = te_shared_ptr_new<ResourceHandleData>();
            this->_handleData->uuid = uuid;
        }

        /** Constructs a new valid handle for the provided resource with the provided UUID. */
        TResourceHandle(const SPtr<T> ptr, const te::UUID& uuid)
        {
            this->_handleData = te_shared_ptr_new<ResourceHandleData>();
            this->SetHandleData(ptr, uuid);
        }

        /** Replaces the internal handle data pointer, effectively transforming the handle into a different handle. */
        void SetHandleData(const SPtr<ResourceHandleData>& data)
        {
            this->_handleData = data;
        }

        using ResourceHandleBase::SetHandleData;
    };

    /** Checks if two handles point to the same resource. */
    template<class _Ty1, class _Ty2>
    bool operator==(const TResourceHandle<_Ty1>& _Left, const TResourceHandle<_Ty2>& _Right)
    {
        if (_Left.GetHandleData() != nullptr && _Right.GetHandleData() != nullptr)
        {
            return _Left.GetHandleData()->data == _Right.GetHandleData()->data;
        }

        return _Left.GetHandleData() == _Right.GetHandleData();
    }

    /** Checks if a handle is null. */
    template<class _Ty1, class _Ty2>
    bool operator==(const TResourceHandle<_Ty1>& _Left, std::nullptr_t _Right)
    {
        return _Left.GetHandleData() == nullptr || _Left.GetHandleData()->uuid.Empty();
    }

    template<class _Ty1, class _Ty2>
    bool operator!=(const TResourceHandle<_Ty1>& _Left, const TResourceHandle<_Ty2>& _Right)
    {
        return (!(_Left == _Right));
    }

    /** Casts one resource handle to another. */
    template<class _Ty1, class _Ty2>
    TResourceHandle<_Ty1> static_resource_cast(const TResourceHandle<_Ty2>& other)
    {
        TResourceHandle<_Ty1> handle;
        handle.SetHandleData(other.GetHandleData());

        return handle;
    }

    template <typename T>
    using ResourceHandle = TResourceHandle<T>;
}
