#pragma once

#include "Prerequisites/TePlatformDefines.h"
#include "Prerequisites/TeTypes.h"
#include "Prerequisites/TeStdHeaders.h"

#include <atomic>
#include <limits>
#include <new>


namespace te
{
    /**
     * Frame allocator. Performs very fast allocations but can only free all of its memory at once. Perfect for allocations
     * that last just a single frame.
     */
    class TE_UTILITY_EXPORT FrameAllocator
    {
    private:
        /** A single block of memory within a frame allocator. */
        class MemBlock
        {
        public:
            MemBlock(UINT32 size) : _size(size) { }

            ~MemBlock() = default;

            /** Allocates a piece of memory within the block. Caller must ensure the block has enough empty space. */
            UINT8* Allocate(UINT32 amount);

            /** Releases all allocations within a block but doesn't actually free the memory. */
            void Clear();

            UINT8* _data = nullptr;
            UINT32 _freePtr = 0;
            UINT32 _size;
        };

    public:
        FrameAllocator(UINT32 blockSize = 1024 * 1024);
        ~FrameAllocator();

        /**
         * Allocates a new block of memory of the specified size.
         *
         * @param[in]	amount	Amount of memory to allocate, in bytes.
         *
         * @note	Not thread safe.
         */
        UINT8* Allocate(UINT32 amount);

        /**
         * Allocates a new block of memory of the specified size aligned to the specified boundary. If the aligment is less
         * or equal to 16 it is more efficient to use the allocAligned16() alternative of this method.
         *
         * @param[in]	amount		Amount of memory to allocate, in bytes.
         * @param[in]	alignment	Alignment of the allocated memory. Must be power of two.
         *
         * @note	Not thread safe.
         */
        UINT8* AllocateAligned(UINT32 amount, UINT32 alignment);

        /**
         * Allocates and constructs a new object.
         *
         * @note	Not thread safe.
         */
        template<class T, class... Args>
        T* Construct(Args &&...args)
        {
            return new ((T*)Allocate(sizeof(T))) T(std::forward<Args>(args)...);
        }

        /**
         * Destructs and deallocates an object.
         *
         * @note	Not thread safe.
         */
        template<class T>
        void Destruct(T* data)
        {
            data->~T();
            Free((UINT8*)data);
        }

        /**
         * Deallocates a previously allocated block of memory.
         *
         * @note
         * No deallocation is actually done here. This method is only used for debug purposes so it is easier to track
         * down memory leaks and corruption.
         * @note
         * Thread safe.
         */
        void Free(UINT8* data);

        /**
         * Deallocates and destructs a previously allocated object.
         *
         * @note
         * No deallocation is actually done here. This method is only used to call the destructor and for debug purposes
         * so it is easier to track down memory leaks and corruption.
         * @note
         * Thread safe.
         */
        template<class T>
        void Free(T* obj)
        {
            if (obj != nullptr)
                obj->~T();

            Free((UINT8*)obj);
        }

        /** Starts a new frame. Next call to clear() will only clear memory allocated past this point. */
        void MarkFrame();

        /**
         * Deallocates all allocated memory since the last call to markFrame() (or all the memory if there was no call
         * to markFrame()).
         *
         * @note	Not thread safe.
         */
        void Clear();

    private:
        UINT32 _blockSize;
        Vector<MemBlock*> _blocks;
        MemBlock* _freeBlock;
        UINT32 _nextBlockIdx;
        std::atomic<UINT32> _totalAllocBytes;
        void* _lastFrame = nullptr;

        /**
         * Allocates a dynamic block of memory of the wanted size. The exact allocation size might be slightly higher in
         * order to store block meta data.
         */
        MemBlock* AllocateBlock(UINT32 wantedSize);

        /** Frees a memory block. */
        void DeallocateBlock(MemBlock* block);
    };

    /**
     * Version of FrameAlloc that allows blocks size to be provided through the template argument instead of the
     * constructor. */
    template<int BlockSize>
    class TFrameAllocator : public FrameAllocator
    {
    public:
        TFrameAllocator()
            : FrameAllocator(BlockSize)
        { }
    };

    /** Allocator for the standard library that internally uses a frame allocator. */
    template <class T>
    class StdFrameAlloc
    {
    public:
        typedef T value_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;

        StdFrameAlloc() noexcept = default;

        StdFrameAlloc(FrameAllocator* alloc) noexcept
            : _frameAllocator(alloc)
        { }

        template<class U> StdFrameAlloc(const StdFrameAlloc<U>& alloc) noexcept
            : _frameAllocator(alloc._frameAllocator)
        { }

        template<class U> bool operator==(const StdFrameAlloc<U>&) const noexcept { return true; }
        template<class U> bool operator!=(const StdFrameAlloc<U>&) const noexcept { return false; }
        template<class U> class rebind { public: typedef StdFrameAlloc<U> other; };

        /** Allocate but don't initialize number elements of type T.*/
        T* allocate(const size_t num) const
        {
            if (num == 0)
                return nullptr;

            if (num > static_cast<size_t>(-1) / sizeof(T))
                return nullptr; // Error

            void* const pv = _frameAllocator->Allocate((UINT32)(num * sizeof(T)));
            if (!pv)
                return nullptr; // Error

            return static_cast<T*>(pv);
        }

        /** Deallocate storage p of deleted elements. */
        void deallocate(T* p, size_t num) const noexcept
        {
            _frameAllocator->Free((UINT8*)p);
        }

        FrameAllocator* _frameAllocator = nullptr;

        size_t max_size() const { return std::numeric_limits<size_type>::max() / sizeof(T); }
        void construct(pointer p, const_reference t) { new (p) T(t); }
        void destroy(pointer p) { p->~T(); }
        template<class U, class... Args>
        void construct(U* p, Args&&... args) { new(p) U(std::forward<Args>(args)...); }
    };

    /** Return that all specializations of this allocator are interchangeable. */
    template <class T1, class T2>
    bool operator== (const StdFrameAlloc<T1>&,
        const StdFrameAlloc<T2>&) throw() {
        return true;
    }

    /** Return that all specializations of this allocator are interchangeable. */
    template <class T1, class T2>
    bool operator!= (const StdFrameAlloc<T1>&,
        const StdFrameAlloc<T2>&) throw() {
        return false;
    }

    /**
     * Returns a global, application wide FrameAllocator. Each thread gets its own frame allocator.
     */
    TE_UTILITY_EXPORT FrameAllocator& gFrameAllocator();

    /**
     * Allocates some memory using the global frame allocator.
     *
     * @param[in]	numBytes	Number of bytes to allocate.
     */
    TE_UTILITY_EXPORT UINT8* te_frame_allocate(UINT32 numBytes);

    /**
     * Allocates the specified number of bytes aligned to the provided boundary, using the global frame allocator. Boundary
     * is in bytes and must be a power of two.
     */
    TE_UTILITY_EXPORT UINT8* te_frame_allocate_aligned(UINT32 count, UINT32 align);

    /**
     * Deallocates memory allocated with the global frame allocator.
     *
     * @note	Must be called on the same thread the memory was allocated on.
     */
    TE_UTILITY_EXPORT void te_frame_free(void* data);

    /**
     * Frees memory previously allocated with te_frame_alloc_aligned().
     *
     * @note	Must be called on the same thread the memory was allocated on.
     */
    TE_UTILITY_EXPORT void te_frame_free_aligned(void* data);

    /**
     * Allocates enough memory to hold the object of specified type using the global frame allocator, but does not
     * construct the object.
     */
    template<class T>
    T* te_frame_allocate()
    {
        return (T*)te_frame_allocate(sizeof(T));
    }

    /**
     * Allocates enough memory to hold N objects of specified type using the global frame allocator, but does not
     * construct the object.
     */
    template<class T>
    T* te_frame_allocate(UINT32 count)
    {
        return (T*)te_frame_allocate(sizeof(T) * count);
    }

    /**
     * Allocates enough memory to hold the object(s) of specified type using the global frame allocator,
     * and constructs them.
     */
    template<class T>
    T* te_frame_new(UINT32 count = 0)
    {
        T* data = te_frame_allocate<T>(count);

        for (unsigned int i = 0; i < count; i++)
            new ((void*)&data[i]) T;

        return data;
    }

    /**
     * Allocates enough memory to hold the object(s) of specified type using the global frame allocator, and constructs them.
     */
    template<class T, class... Args>
    T* te_frame_new(Args &&...args, UINT32 count = 0)
    {
        T* data = te_frame_allocate<T>(count);

        for (unsigned int i = 0; i < count; i++)
            new ((void*)&data[i]) T(std::forward<Args>(args)...);

        return data;
    }

    /**
     * Destructs and deallocates an object allocated with the global frame allocator.
     */
    template<class T>
    void te_frame_delete(T* data)
    {
        data->~T();

        te_frame_free((UINT8*)data);
    }

    /**
     * Destructs and deallocates an array of objects allocated with the global frame allocator.
     */
    template<class T>
    void te_frame_delete(T* data, UINT32 count)
    {
        for (unsigned int i = 0; i < count; i++)
            data[i].~T();

        te_frame_free((UINT8*)data);
    }

    /** @copydoc FrameAllocator::MarkFrame */
    TE_UTILITY_EXPORT void te_frame_mark();

    /** @copydoc FrameAllocator::Clear */
    TE_UTILITY_EXPORT void te_frame_clear();

    /** Vector allocated with a frame allocator. */
    template <typename T, typename A = StdAllocator<T, FrameAllocator>>
    using FrameVector = std::vector < T, A >;
}
