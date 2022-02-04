#pragma once

namespace te
{
    /** Interface that prevents copies be made of any type that implements it. */
    class TE_UTILITY_EXPORT NonCopyable
    {
    public:
        NonCopyable() = default;
        virtual ~NonCopyable() = default;

    private:
        NonCopyable(const NonCopyable&) = delete;
        NonCopyable& operator=(const NonCopyable&) = delete;
    };
}
