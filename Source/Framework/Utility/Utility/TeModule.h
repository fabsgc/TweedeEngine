#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"

#if TE_PLATFORM == TE_PLATFORM_WIN32 && TE_COMPILER == TE_COMPILER_GNUC
#   ifndef TE_MODULE_STATIC_MEMBER
#   define TE_MODULE_STATIC_MEMBER(class_name)                                                          \
    bool class_name::StartedUp   = false;                                                               \
    bool class_name::Destroyed   = false;                                                               \
    class_name* class_name::Inst = nullptr;                                                             \
                                                                                                        \
    bool& class_name::IsStartedUp() { return StartedUp; }                                               \
    bool& class_name::IsDestroyed() { return Destroyed; }                                               \
    class_name*& class_name::_instance() { return Inst; }                                               \
    class_name& class_name::Instance()                                                                  \
    {                                                                                                   \
        if (!IsStartedUp())                                                                             \
            TE_ASSERT_ERROR(false, "Trying to access a module but it hasn't been started up yet.");     \
        if (IsDestroyed())                                                                              \
            TE_ASSERT_ERROR(false, "Trying to access a destroyed module.");                             \
        return *_instance();                                                                            \
    }                                                                                                   \
                                                                                                        \
    class_name* class_name::InstancePtr()                                                               \
    {                                                                                                   \
        if (!IsStartedUp())                                                                             \
            TE_ASSERT_ERROR(false, "Trying to access a module but it hasn't been started up yet.");     \
        if (IsDestroyed())                                                                              \
            TE_ASSERT_ERROR(false, "Trying to access a destroyed module.");                             \
        return _instance();                                                                             \
    }                                                                                                   \
                                                                                                        \
    void class_name::ShutDown()                                                                         \
    {                                                                                                   \
        if (IsDestroyed())                                                                              \
            TE_ASSERT_ERROR(false, "Trying to shut down an already shut down module.");                 \
        if (!IsStartedUp())                                                                             \
            TE_ASSERT_ERROR(false, "Trying to shut down a module which was never started.");            \
        (_instance())->OnShutDown();                                                                    \
        _instance()->~class_name();                                                                     \
        ::free(_instance());                                                                            \
        IsDestroyed() = true;                                                                           \
    }                                                                                                   \
                                                                                                        \
    bool class_name::IsStarted() { return IsStartedUp() && !IsDestroyed(); }
#   endif

#   ifndef TE_MODULE_STATIC_HEADER_MEMBER
#   define TE_MODULE_STATIC_HEADER_MEMBER(class_name)                                                   \
                                                                                                        \
    static bool StartedUp;                                                                              \
    static bool Destroyed;                                                                              \
    static class_name* Inst;                                                                            \
                                                                                                        \
    static class_name*& _instance();                                                                    \
    static bool& IsStartedUp();                                                                         \
    static bool& IsDestroyed();                                                                         \
                                                                                                        \
                                                                                                        \
    static class_name& Instance();                                                                      \
    static class_name* InstancePtr();                                                                   \
    static void ShutDown();                                                                             \
    static bool IsStarted();                                                                            \
                                                                                                        \
    template<class ...Args>                                                                             \
    static void StartUp(Args&& ...args)                                                                 \
    {                                                                                                   \
        if (IsStartedUp())                                                                              \
            TE_ASSERT_ERROR(false, "Trying to start an already started module.");                       \
        _instance() = new class_name(std::forward<Args>(args)...);                                      \
        IsStartedUp() = true;                                                                           \
        ((class_name*)_instance())->OnStartUp();                                                        \
    }                                                                                                   \
                                                                                                        \
    template<class SubType, class ...Args>                                                              \
    static void StartUp(Args&& ...args)                                                                 \
    {                                                                                                   \
        static_assert(std::is_base_of<class_name, SubType>::value,                                      \
            "Provided type is not derived from type the Module is initialized with.");                  \
        if (IsStartedUp())                                                                              \
            TE_ASSERT_ERROR(false, "Trying to start an already started module.");                       \
        _instance() = new SubType(std::forward<Args>(args)...);                                         \
        IsStartedUp() = true;                                                                           \
        ((class_name*)_instance())->OnStartUp();                                                        \
    }
#   endif
#else
#   ifndef TE_MODULE_STATIC_MEMBER
#   define TE_MODULE_STATIC_MEMBER(class_name)
#   endif

#   ifndef TE_MODULE_STATIC_HEADER_MEMBER
#   define TE_MODULE_STATIC_HEADER_MEMBER(class_name)
#   endif
#endif

namespace te
{
    /**
     * Represents one engine module. Essentially it is a specialized type of singleton. Module must be manually started up
     * and shut down before and after use.
     */
    template <class T>
    class Module
    {
    public:
#if !(TE_PLATFORM == TE_PLATFORM_WIN32 && TE_COMPILER == TE_COMPILER_GNUC)
        /** Returns a reference to the module instance. Module has to have been started up first otherwise an assert will be thrown. */
        static T& Instance()
        {
            if (!IsStartedUp())
            {
                TE_ASSERT_ERROR(false,
                    "Trying to access a module but it hasn't been started up yet.");
            }

            if (IsDestroyed())
            {
                TE_ASSERT_ERROR(false,
                    "Trying to access a destroyed module.");
            }

            return *_instance();
        }

        /** Returns a pointer to the module instance. Module has to have been started up first otherwise an assert will be thrown. */
        static T* InstancePtr()
        {
            if (!IsStartedUp())
            {
                TE_ASSERT_ERROR(false, "Trying to access a module but it hasn't been started up yet.");
            }

            if (IsDestroyed())
            {
                TE_ASSERT_ERROR(false, "Trying to access a destroyed module.");
            }

            return _instance();
        }

        /** Constructs and starts the module using the specified parameters. */
        template<class ...Args>
        static void StartUp(Args&& ...args)
        {
            if (IsStartedUp())
            {
                TE_ASSERT_ERROR(false, "Trying to start an already started module.");
            }

            _instance() = new T(std::forward<Args>(args)...);
            IsStartedUp() = true;

            ((Module*)_instance())->OnStartUp();
        }

        /** Constructs and starts a specialized type of the module. Provided type must derive from type the Module is initialized with. */
        template<class SubType, class ...Args>
        static void StartUp(Args&& ...args)
        {
            static_assert(std::is_base_of<T, SubType>::value, "Provided type is not derived from type the Module is initialized with.");

            if (IsStartedUp())
            {
                TE_ASSERT_ERROR(false, "Trying to start an already started module.");
            }

            _instance() = new SubType(std::forward<Args>(args)...);
            IsStartedUp() = true;

            ((Module*)_instance())->OnStartUp();
        }

        /** Shuts down this module and frees any resources it is using. */
        static void ShutDown()
        {
            if (IsDestroyed())
            {
                TE_ASSERT_ERROR(false, "Trying to shut down an already shut down module.");
            }

            if (!IsStartedUp())
            {
                TE_ASSERT_ERROR(false, "Trying to shut down a module which was never started.");
            }

            ((Module*)_instance())->OnShutDown();

            te_delete(_instance());
            IsDestroyed() = true;
        }

        /** Query if the module has been started. */
        static bool IsStarted()
        {
            return IsStartedUp() && !IsDestroyed();
        }
#endif

    protected:
        Module() = default;

        virtual ~Module() = default;

        /*
         * The notion of copying or moving a singleton is rather nonsensical.
         */
        Module(Module&&) = delete;
        Module(const Module&) = delete;
        Module& operator=(Module&&) = delete;
        Module& operator=(const Module&) = delete;

        /**
         * Override if you want your module to be notified once it has been constructed and started.
         * @note	Useful when your module is polymorphic and you cannot perform some implementation specific
         *			initialization in constructor itself.
         */
        virtual void OnStartUp() {}

        /**
         * Override if you want your module to be notified just before it is deleted.
         * @note	Useful when your module is polymorphic and you might want to perform some kind of clean up perhaps
         *			overriding that of a base class.
         */
        virtual void OnShutDown() {}

#if !(TE_PLATFORM == TE_PLATFORM_WIN32 && TE_COMPILER == TE_COMPILER_GNUC)
        /** Returns a singleton instance of this module. */
        static T*& _instance()
        {
            static T* inst = nullptr;
            return inst;
        }

        /** Checks if the Module has been shut down. */
        static bool& IsDestroyed()
        {
            static bool inst = false;
            return inst;
        }

        /** Checks if the Module has been started up. */
        static bool& IsStartedUp()
        {
            static bool inst = false;
            return inst;
        }
#endif
    };
}
