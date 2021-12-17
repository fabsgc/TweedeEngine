#pragma once

#ifdef __BORLANDC__
#   define __STD_ALGORITHM
#endif

#include <memory>

// STL containers
#include <vector>
#include <stack>
#include <map>
#include <set>
#include <list>
#include <deque>
#include <queue>
#include <array>
#include <unordered_map>
#include <unordered_set>

// STL algorithms & functions


extern "C" {
#   include <sys/types.h>
#   include <sys/stat.h>
}

#if TE_PLATFORM == TE_PLATFORM_WIN32
#  undef min
#  undef max
#   if !defined(NOMINMAX) && defined(_MSC_VER)
#       define NOMINMAX // required to stop windows.h messing up std::min
#   endif
#  if defined( __MINGW32__ )
#    include <unistd.h>
#  endif
#endif

#if TE_PLATFORM == TE_PLATFORM_LINUX
extern "C" {
#   include <unistd.h>
#   include <dlfcn.h>
}

#   include <malloc.h>
#endif

#if TE_PLATFORM == TE_PLATFORM_WIN32
#   include <Windows.h>
#   include <fcntl.h>
#   include <io.h>
#   include <iphlpapi.h>
#   include <intrin.h>

#   ifndef UNICODE
//#       define UNICODE
#   endif

#   ifndef _UNICODE
//#       define _UNICODE
#   endif
#endif

#if TE_PLATFORM == TE_PLATFORM_WIN32
#   define TE_SLEEP(ms) Sleep(ms)
#elif TE_PLATFORM == TE_PLATFORM_LINUX
#   define TE_SLEEP(ms) usleep(ms)
#else
#   define TE_SLEEP(ms) usleep(ms)
#endif

namespace te
{
    /* ###################################################################
    *  ############# MEMORY ALLOCATOR BASE ###############################
    *  ################################################################ */

#if TE_PLATFORM == TE_PLATFORM_WIN32
    inline void* PlatformAlignedAllocate16(size_t size)
    {
        return _aligned_malloc(size, 16);
    }

    inline void PlatformAlignedFree16(void* ptr)
    {
        _aligned_free(ptr);
    }

    inline void* PlatformAlignedAllocate(size_t size, size_t alignment)
    {
        return _aligned_malloc(size, alignment);
    }

    inline void PlatformAlignedFree(void* ptr)
    {
        _aligned_free(ptr);
    }
#elif TE_PLATFORM == TE_PLATFORM_LINUX
    inline void* PlatformAlignedAllocate16(size_t size)
    {
        return ::memalign(16, size);
    }

    inline void PlatformAlignedFree16(void* ptr)
    {
        ::free(ptr);
    }

    inline void* PlatformAlignedAllocate(size_t size, size_t alignment)
    {
        return ::memalign(alignment, size);
    }

    inline void PlatformAlignedFree(void* ptr)
    {
        ::free(ptr);
    }
#endif

    /**
    * Memory allocator providing a generic implementation. Specialize for specific categories as needed.
    */
    class MemoryAllocator
    {
    public:
        static void* Allocate(size_t bytes)
        {
            void* p = ::malloc(bytes);

            if(p == 0) 
                throw std::bad_alloc();

            return p;
        }

        static void Deallocate(void* ptr)
        {
            ::free(ptr);
        }

        /**
         * Allocates @p bytes and aligns them to the specified boundary (in bytes). If the aligment is less or equal to
         * 16 it is more efficient to use the allocateAligned16() alternative of this method. Alignment must be power of two.
         */
        static void* AllocateAligned(size_t bytes, size_t alignment)
        {
            return PlatformAlignedAllocate(bytes, alignment);
        }

        /** Allocates @p bytes and aligns them to a 16 byte boundary. */
        static void* AllocateAligned16(size_t bytes)
        {
            return PlatformAlignedAllocate16(bytes);
        }

        /** Frees memory allocated with allocateAligned */
        static void FreeAligned(void* ptr)
        {
            PlatformAlignedFree(ptr);
        }

        /** Frees memory allocated with allocateAligned16 */
        static void FreeAligned16(void* ptr)
        {
            PlatformAlignedFree16(ptr);
        }
    };

    /**
    * Allocates the specified number of bytes.
    */
    inline void* te_allocate(uint32_t count)
    {
        return MemoryAllocator::Allocate(count);
    }

    /**
    * Allocates enough bytes to hold the specified type, but doesn't construct it.
    */
    template<class T>
    inline T* te_allocate(uint32_t count)
    {
        return (T*)MemoryAllocator::Allocate(count);
    }

    /**
     * Allocates the specified number of bytes aligned to the provided boundary. Boundary is in bytes and must be a power
     * of two.
     */
    inline void* te_allocate_aligned(size_t count, size_t align)
    {
        return MemoryAllocator::AllocateAligned(count, align);
    }

    /** Allocates the specified number of bytes aligned to a 16 bytes boundary. */
    inline void* te_allocate_aligned16(size_t count)
    {
        return MemoryAllocator::AllocateAligned16(count);
    }

    /** Frees memory previously allocated with bs_alloc_aligned(). */
    inline void te_free_aligned(void* ptr)
    {
        MemoryAllocator::FreeAligned(ptr);
    }

    /** Frees memory previously allocated with bs_alloc_aligned16(). */
    inline void te_free_aligned16(void* ptr)
    {
        MemoryAllocator::FreeAligned16(ptr);
    }

    /**
    * Allocates enough bytes to hold the specified type, but doesn't construct it.
    */
    template<class T>
    inline T* te_allocate()
    {
        return (T*)MemoryAllocator::Allocate(sizeof(T));
    }

    /** Allocates enough bytes to hold an array of @p count elements the specified type, but doesn't construct them. */
    template<class T>
    T* te_allocateN(size_t count)
    {
        return (T*)MemoryAllocator::Allocate(count * sizeof(T));
    }

    /** Creates and constructs an array of @p count elements. */
    template<class T>
    T* te_newN(uint32_t count)
    {
        T* ptr = (T*)te_allocate<T>(sizeof(T) * count);

        for (size_t i = 0; i < count; ++i)
            new (&ptr[i]) T;

        return ptr;
    }

    /**
    * Create a new object with the specified allocator and the specified parameters.
    */
    template<class Type, class... Args>
    inline Type* te_new(Args &&...args)
    {
        return new (te_allocate<Type>(sizeof(Type))) Type(std::forward<Args>(args)...);
    }

    /**
    * Frees all the bytes allocated at the specified location.
    */
    inline void te_deallocate(void* ptr)
    {
        MemoryAllocator::Deallocate(ptr);
    }

    /** Destructs and frees the specified object. */
    template<class T>
    inline void te_delete(T* ptr)
    {
        (ptr)->~T();
        MemoryAllocator::Deallocate(ptr);
    }

    /** Destructs and frees the specified object if not null. */
    template<class T>
    inline void te_safe_delete(T* ptr)
    {
        if (ptr)
        {
            (ptr)->~T();
            MemoryAllocator::Deallocate(ptr);
        }
    }

    /** Callable struct that acts as a proxy for te_delete */
    template<class T>
    struct Deleter
    {
        constexpr Deleter() noexcept = default;

        /** Constructor enabling deleter conversion and therefore polymorphism with smart points (if they use the same allocator). */
        template <class T2, std::enable_if_t<std::is_convertible<T2*, T*>::value, int> = 0>
        constexpr Deleter(const Deleter<T2>& other) noexcept { }

        void operator()(T* ptr) const
        {
            te_delete<T>(ptr);
        }
    };

    /** Destructs and frees the specified array of objects. */
    template<class T>
    void te_deleteN(T* ptr, size_t count)
    {
        for (size_t i = 0; i < count; ++i)
        {
            ptr[i].~T();
        }

        MemoryAllocator::Deallocate(ptr);
    }

    /** Frees all the bytes allocated at the specified location. */
    inline void te_free(void* ptr)
    {
        MemoryAllocator::Deallocate(ptr);
    }

    /**
     * Hash for enum types, to be used instead of std::hash<T> when T is an enum.
     *
     * Until C++14, std::hash<T> is not defined if T is a enum (see
     * http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-defects.html#2148).  But
     * even with C++14, as of october 2016, std::hash for enums is not widely
     * implemented by compilers, so here when T is a enum, we use EnumClassHash
     * instead of std::hash. (For instance, in te::hash_combine(), or
     * te::UnorderedMap.)
     */
    struct EnumClassHash
    {
        template <typename T>
        constexpr std::size_t operator()(T t) const
        {
            return static_cast<std::size_t>(t);
        }
    };

    struct PairHash {
        template <class T1, class T2>
        std::size_t operator () (const std::pair<T1,T2> &p) const {
            auto h1 = std::hash<T1>{}(p.first);
            auto h2 = std::hash<T2>{}(p.second);

            // Mainly for demonstration purposes, i.e. works but is overly simple
            // In the real world, use sth. like boost.hash_combine
            return h1 ^ h2;  
        }
    };

    /** Allocator for the standard library that internally uses framework memory allocator. */
    template <class T, class Allocator>
    class StdAllocator
    {
    public:
        using value_type = T;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        constexpr StdAllocator() = default;
        constexpr StdAllocator(StdAllocator&&) = default;
        constexpr StdAllocator(const StdAllocator&) = default;
        template<class U, class Alloc2> constexpr StdAllocator(const StdAllocator<U, Alloc2>&) { };
        template<class U, class Alloc2> constexpr bool operator==(const StdAllocator<U, Alloc2>&) const noexcept { return true; }
        template<class U, class Alloc2> constexpr bool operator!=(const StdAllocator<U, Alloc2>&) const noexcept { return false; }

        template<class U> class rebind { public: using other = StdAllocator<U, Allocator>; };

        /** Allocate but don't initialize number elements of type T. */
        static T* allocate(const size_t num)
        {
            if (num == 0)
                return nullptr;

            if (num > max_size())
                return nullptr; // Error

            void* const pv = te_allocate<Allocator>((uint32_t)(num * sizeof(T)));
            if (!pv)
                return nullptr; // Error

            return static_cast<T*>(pv);
        }

        /** Deallocate storage p of deleted elements. */
        static void deallocate(pointer p, size_type)
        {
            te_free(p);
        }

        static constexpr size_t max_size() { return std::numeric_limits<size_type>::max() / sizeof(T); }
        static constexpr void destroy(pointer p) { p->~T(); }

        template<class... Args>
        static void construct(pointer p, Args&&... args) { new(p) T(std::forward<Args>(args)...); }
    };

    /* ###################################################################
    *  ############# STL CONTAINER SNIPPETS ##############################
    *  ################################################################ */

    template <typename T>
    using SPtr = std::shared_ptr<T>;

    template <typename T>
    using WPtr = std::weak_ptr<T>;

    template <typename T>
    using UPtr = std::unique_ptr<T>;

    /** Hasher that handles custom enums automatically. */
    template <typename Key>
    using HashType = typename std::conditional<std::is_enum<Key>::value, EnumClassHash, std::hash<Key>>::type;

    /** Double ended queue. Allows for fast insertion and removal at both its beggining and end. */
    template <typename T>
    using Deque = std::deque<T>;

    /** Dynamically sized array that stores element contigously. */
    template <typename T>
    using Vector = std::vector<T>;

    /**
     * Container that supports constant time insertion and removal for elements with known locations, but without fast
     * random access to elements. Internally implemented as a doubly linked list. Use ForwardList if you do not need
     * reverse iteration.
     */
    template <typename T>
    using List = std::list<T>;

    /** First-in, last-out data structure. */
    template <typename T>
    using Stack = std::stack<T, std::deque<T>>;

    /** First-in, first-out data structure. */
    template <typename T>
    using Queue = std::queue<T, std::deque<T>>;

    /** An associative container containing an ordered set of elements. */
    template <typename T, typename A = std::allocator<T>>
    using Set = std::set<T, A>;

    /** A combination of two elements. */
    template <typename L, typename R>
    using Pair = std::pair<L, R>;

    /** An associative container containing an ordered set of key-value pairs. */
    template <typename K, typename V, typename P = std::less<K>>
    using Map = std::map<K, V, P>;

    /** An associative container containing an ordered set of elements where multiple elements can have the same key. */
    template <typename T, typename P = std::less<T>>
    using MultiSet = std::multiset<T, P>;

    /** An associative container containing an ordered set of key-value pairs where multiple elements can have the same key. */
    template <typename K, typename V, typename P = std::less<K>>
    using MultiMap = std::multimap<K, V, P>;

    /** An associative container containing an unordered set of elements. Usually faster than Set for larger data sets. */
    template <typename T, typename H = HashType<T>, typename C = std::equal_to<T>>
    using UnorderedSet = std::unordered_set<T, H, C>;

    /** An associative container containing an ordered set of key-value pairs. Usually faster than Map for larger data sets. */
    template <typename K, typename V, typename H = HashType<K>, typename C = std::equal_to<K>>
    using UnorderedMap = std::unordered_map<K, V, H, C>;

    /**
     * An associative container containing an ordered set of key-value pairs where multiple elements can have the same key.
     * Usually faster than MultiMap for larger data sets.
     */
    template <typename K, typename V, typename H = HashType<K>, typename C = std::equal_to<K>>
    using UnorderedMultimap = std::unordered_multimap<K, V, H, C>;

    /* ###################################################################
    *  ############# SHARED PTR INSTANTIATION ############################
    *  ################################################################ */

    /**
    * Create a new shared pointer using a custom allocator category.
    */
    template<class Type, class... Args>
    SPtr<Type> te_shared_ptr_new(Args &&... args)
    {
        return std::allocate_shared<Type>(std::allocator<Type>(), std::forward<Args>(args)...);
    }

    /**
     * Create a new shared pointer from a previously constructed object.
     * Pointer specific data will be allocated using the provided allocator category.
     */
    template<typename Type, typename Delete = Deleter<Type>>
    SPtr<Type> te_shared_ptr(Type* data, Delete del = Delete())
    {
        return SPtr<Type>(data, std::move(del));
    }

    /* ###################################################################
    *  ############# UNIQUE PTR INSTANTIATION ############################
    *  ################################################################ */

    /**
     * Create a new unique pointer from a previously constructed object.
     * Pointer specific data will be allocated using the provided allocator category.
     */
    template<typename Type>
    UPtr<Type> te_unique_ptr(Type* data)
    {
        return std::unique_ptr<Type>(data);
    }

    /**
    * Create a new unique pointer using a custom allocator category.
    */
    template<class Type, class... Args>
    UPtr<Type> te_unique_ptr_new(Args &&... args)
    {
        Type* rawPtr = te_new<Type>(std::forward<Args>(args)...);
        return te_unique_ptr<Type>(rawPtr);
    }
}
