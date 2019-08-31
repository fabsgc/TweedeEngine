#include "Utility/TeTime.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(Time)

    const double Time::MICROSEC_TO_SEC = 1.0 / 1000000.0;

    Time::Time()
        : _run(true)
        , _frameDelta(0.0f)
        , _timeSinceStart(0.0f)
        , _timeSinceStartMs(0)
        , _appStartTime(0)
        , _lastFrameTime(0)
        , _currentFrame(0UL)
    {
        _timer.reset(new Timer());
        _appStartTime = _timer->GetStartMs();
        _lastFrameTime = _timer->GetMicroseconds();
    }

    void Time::Update()
    {
        if (_run)
        {
            UINT64 currentFrameTime = _timer->GetMicroseconds();

            _frameDelta = (float)((currentFrameTime - _lastFrameTime) * MICROSEC_TO_SEC);
            _timeSinceStartMs = (UINT64)(currentFrameTime / 1000);
            _timeSinceStart = _timeSinceStartMs / 1000.0f;

            _lastFrameTime = currentFrameTime;

            _currentFrame.fetch_add(1, std::memory_order_relaxed);
        }
    }

    Time& gTime()
    {
        return Time::Instance();
    }
}
