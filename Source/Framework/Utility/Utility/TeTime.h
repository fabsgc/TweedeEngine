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

        float GetTime() const { return _timeSinceStart; }
        UINT64 GetTimeMs() const { return _timeSinceStartMs; }
        float GetFrameDelta() const { return _frameDelta; }
        UINT64 GetFrameIdx() const { return _currentFrame.load(); }
        UINT64 GetTimePrecise() const { return _timer->GetMicroseconds(); }
        UINT64 GetStartTimeMs() const { return _appStartTime; }
        void Update();
        void Stop() {  _run = false; }
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

    Time& gTime();
}
