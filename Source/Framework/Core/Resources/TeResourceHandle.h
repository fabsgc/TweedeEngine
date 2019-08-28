#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"

namespace te
{
    /**	Data that is shared between all resource handles. */
	struct TE_CORE_EXPORT ResourceHandleData
	{
		SPtr<Resource> resource;
        te::UUID uuid;
		UINT32 refCount = 0;
	};

    /** Manager that handles resource loading. */
	class TE_CORE_EXPORT ResourceHandleBase
	{
	public:
        /**
		 * Releases an internal reference to this resource held by the resources system, if there is one.
		 */
		void Release();

		/** Returns the UUID of the resource the handle is referring to. */
		const te::UUID& GetUUID() const { return _data != nullptr ? _data->uuid : te::UUID::EMPTY; }

        /**	Gets the handle data. For internal use only. */
		const SPtr<ResourceHandleData>& GetHandleData() const { return _data; }

    protected:
        /**	Destroys the resource the handle is pointing to. */
		void Destroy();

        /**
		 * Sets the created flag to true and assigns the resource pointer. Called by the constructors, or if you
		 * constructed just using a UUID, then you need to call this manually before you can access the resource from
		 * this handle.
		 */
		void SetHandleData(const SPtr<Resource>& ptr, const UUID& uuid);

		/**
		 * Clears the created flag and the resource pointer, making the handle invalid until the resource is loaded again
		 * and assigned through setHandleData().
		 */
		void ClearHandleData();

		/** Increments the reference count of the handle. Only to be used by Resources for keeping internal references. */
		void AddInternalRef();

		/** Decrements the reference count of the handle. Only to be used by Resources for keeping internal references. */
		void RemoveInternalRef();

    protected:
        SPtr<ResourceHandleData> _data;
    };

    template <typename T>
	class TResourceHandle : public ResourceHandleBase
	{
	public:
		TResourceHandle() = default;

        /**	Copy constructor. */
		TResourceHandle(const TResourceHandle& other)
		{
			this->_data = other.GetHandleData();
			this->AddRef();
		}

		/** Move constructor. */
		TResourceHandle(TResourceHandle&& other) = default;

		~TResourceHandle()
		{
			this->ReleaseRef();
		}

        /**	Converts a specific handle to generic Resource handle. */
		operator TResourceHandle<Resource>() const
		{
			TResourceHandle<Resource> handle;
			handle.SetHandleData(this->GetHandleData());

			return handle;
		}

		/**
		 * Returns internal resource pointer.
		 */
		T* operator->() const { return Get(); }

		/**
		 * Returns internal resource pointer and dereferences it.
		 */
		T& operator*() const { return *Get(); }

        /**	Copy assignment. */
		TResourceHandle<T>& operator=(const TResourceHandle<T>& rhs)
		{
			SetHandleData(rhs.GetHandleData());
			return *this;
		}

		/**	Move assignment. */
		TResourceHandle& operator=(TResourceHandle&& other)
		{
			if(this == &other)
            {
				return *this;
            }

			this->ReleaseRef();
			this->_data = std::exchange(other._data, nullptr);

			return *this;
		}

        /**
		 * Returns internal resource pointer and dereferences it.
		 */
		T* Get() const
		{
			return reinterpret_cast<T*>(this->_data->resource.get());
		}

        /**
		 * Returns the internal shared pointer to the resource.
		 */
		SPtr<T> GetInternalPtr() const
		{
			return std::static_pointer_cast<T>(this->_data->resource);
		}

    protected:
        void AddRef()
        {
            if (_data)
            {
                _data->refCount++;
            }
        };

        void ReleaseRef()
        {
            if (_data)
            {
                if (_data->refCount == 1)
                {
                    Destroy();
                }
            }
        };

        /**
		 * Constructs a new valid handle for the provided resource with the provided UUID.
		 * @note	Handle will take ownership of the provided resource pointer, so make sure you don't delete it elsewhere.
		 */
		explicit TResourceHandle(T* ptr, const te::UUID& uuid)
			: ResourceHandleBase()
		{
			this->_data = te_shared_ptr_new<ResourceHandleData>();
			this->AddRef();

			this->SetHandleData(SPtr<Resource>(ptr), uuid);
		}

        /**
		 * Constructs an invalid handle with the specified UUID. You must call setHandleData() with the actual resource
		 * pointer to make the handle valid.
		 */
		TResourceHandle(const te::UUID& uuid)
		{
			this->_data = te_shared_ptr_new<ResourceHandleData>();
			this->_data->uuid = uuid;

			this->AddRef();
		}

        /**	Constructs a new valid handle for the provided resource with the provided UUID. */
		TResourceHandle(const SPtr<T> ptr, const te::UUID& uuid)
		{
			this->_data = te_shared_ptr_new<ResourceHandleData>();
			this->AddRef();

			this->SetHandleData(ptr, uuid);
		}

		/**	Replaces the internal handle data pointer, effectively transforming the handle into a different handle. */
		void SetHandleData(const SPtr<ResourceHandleData>& data)
		{
			this->ReleaseRef();
			this->_data = data;
			this->AddRef();
		}
    };

    /**	Checks if two handles point to the same resource. */
	template<class _Ty1, class _Ty2>
	bool operator==(const TResourceHandle<_Ty1>& _Left, const TResourceHandle<_Ty2>& _Right)
	{
		if(_Left.GetHandleData() != nullptr && _Right.GetHandleData() != nullptr)
			return _Left.GetHandleData()->mPtr == _Right.GetHandleData()->mPtr;

		return _Left.GetHandleData() == _Right.GetHandleData();
	}

    template<class _Ty1, class _Ty2>
	bool operator!=(const TResourceHandle<_Ty1>& _Left, const TResourceHandle<_Ty2>& _Right)
	{
		return (!(_Left == _Right));
	}

    /**	Casts one resource handle to another. */
	template<class _Ty1, class _Ty2>
	TResourceHandle<_Ty1> static_resource_cast(const TResourceHandle<_Ty2>& other)
	{
		TResourceHandle<_Ty1, false> handle;
		handle.SetHandleData(other.GetHandleData());

		return handle;
	}

    template <typename T>
	using ResourceHandle = TResourceHandle<T>;
}