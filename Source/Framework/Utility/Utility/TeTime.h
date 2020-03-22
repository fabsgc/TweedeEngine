#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"
#include "Utility/TeModule.h"
#include "Utility/TeTimer.h"

namespace te
{
    class TE_UTILITY_EXPORT Time : public Module<Time>
    {
    public:
        Time();
        ~Time() {}

        TE_MODULE_STATIC_HEADER_MEMBER(Time)

        /**
         * Gets the time elapsed since application start. Only gets updated once per frame.
         * @return	The time since application start, in seconds.
         */
        float GetTime() const { return _timeSinceStart; }

        /**
         * Gets the time elapsed since application start. Only gets updated once per frame.
         * @return	The time since application start, in miliseconds.
         */
        UINT64 GetTimeMs() const { return _timeSinceStartMs; }

        /**
         * Gets the time since last frame was executed. Only gets updated once per frame.
         * @return	Time since last frame was executed, in seconds.
         */
        float GetFrameDelta() const { return _frameDelta; }

        /**
         * Returns the sequential index of the current frame. First frame is 0.
         * @return	The current frame.
         */
        UINT64 GetFrameIdx() const { return _currentFrame.load(); }

        /**
         * Returns the precise time since application start, in microseconds. Unlike other time methods this is not only
         * updated every frame, but will return exact time at the moment it is called.
         * 		
         * @return	Time in microseconds.
         *
         * @note	
         * You will generally only want to use this for performance measurements and similar. Use non-precise methods in
         * majority of code as it is useful to keep the time value equal in all methods during a single frame.
         */
        UINT64 GetTimePrecise() const { return _timer->GetMicroseconds(); }

        /**
         * Gets the time at which the application was started, counting from system start.
         * @return	The time since system to application start, in milliseconds.
         */
        UINT64 GetStartTimeMs() const { return _appStartTime; }

        /** Called every frame. Should only be called by Application. */
        void Update();

        /** Stop time counting. */
        void Stop() {  _run = false; }

        /** Start time counting. */
        void Start() { _run = true; }

    public:
        static const double MICROSEC_TO_SEC;

    protected:
        bool   _run;
        float  _frameDelta;
        float  _timeSinceStart;
        UINT64 _timeSinceStartMs;

        UINT64 _appStartTime;
        UINT64 _lastFrameTime;
        std::atomic<unsigned long> _currentFrame;

        SPtr<Timer> _timer;
    };

    TE_UTILITY_EXPORT Time& gTime();
}
