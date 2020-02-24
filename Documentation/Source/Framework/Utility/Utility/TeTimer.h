#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"

namespace te
{
    using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;
    using Clock = std::chrono::high_resolution_clock;

    class TE_UTILITY_EXPORT Timer
    {
    public:
        Timer();
        void   Reset();
        UINT64 GetMilliseconds() const;
        UINT64 GetMicroseconds() const;
        UINT64 GetStartMs() const;

    protected:
        Clock     _HRClock;
        TimePoint _startTime;
    };
}
