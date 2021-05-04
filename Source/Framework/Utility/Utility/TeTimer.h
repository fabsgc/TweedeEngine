#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"

#include <chrono>

namespace te
{
    using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;
    using Clock = std::chrono::high_resolution_clock;

    /** Timer class used for querying high precision timers. */
    class TE_UTILITY_EXPORT Timer
    {
    public:
        /** Construct the timer and start timing. */
        Timer();

        /** Reset the timer to zero. */
        void Reset();

        /** Returns time in milliseconds since timer was initialized or last reset. */
        UINT64 GetMilliseconds() const;

        /** Returns time in microseconds since timer was initialized or last reset. */
        UINT64 GetMicroseconds() const;

        /**
         * Returns the time at which the timer was initialized, in milliseconds.
         *
         * @return	Time in milliseconds.
         */
        UINT64 GetStartMs() const;

    protected:
        Clock     _HRClock;
        TimePoint _startTime;
    };
}
