#pragma once

#ifdef __BORLANDC__
#   define __STD_ALGORITHM
#endif

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <atomic>
#include <cstring>
#include <cstdarg>
#include <cmath>
#include <chrono>
#include <memory>
#include <string>
#include <cstdio>
#include <new>
#include <string>
#include <cfloat>

// STL containers
#include <vector>
#include <stack>
#include <map>
#include <string>
#include <set>
#include <list>
#include <deque>
#include <queue>
#include <bitset>
#include <array>
#include <forward_list>
#include <unordered_map>
#include <unordered_set>

// STL algorithms & functions
#include <algorithm>
#include <functional>
#include <limits>
#include <random>

// C++ Stream stuff
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

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

    /**
    * Memory allocator providing a generic implementation. Specialize for specific categories as needed.
    */
    class MemoryAllocator
    {
    public:
        static void* Allocate(size_t bytes)
        {
            return ::malloc(bytes);
        }

        static void Deallocate(void* ptr)
        {
            ::free(ptr);
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
    * Allocates enough bytes to hold the specified type, but doesn't construct it.
    */
    template<class T>
    inline T* te_allocate()
    {
        return (T*)MemoryAllocator::Allocate(sizeof(T));
    }

    /** Creates and constructs an array of @p count elements. */
    template<class T>
    T* te_newN(uint32_t count)
    {
        T* ptr = (T*)te_allocate<T>(sizeof(T) * count);

        for (size_t i = 0; i < count; ++i)
        {
            new (&ptr[i]) T;
        }

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

    /**
     * Container that supports constant time insertion and removal for elements with known locations, but without fast
     * random access to elements. Internally implemented as a singly linked list that doesn't support reverse iteration.
     */
    template <typename T>
    using ForwardList = std::forward_list<T>;

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
        Type* rawPtr = (Type*)te_allocate<Type>(std::forward<Args>(args)...);
        return te_unique_ptr<Type>(rawPtr);
    }
}