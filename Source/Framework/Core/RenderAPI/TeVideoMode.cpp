#include "RenderAPI/TeVideoMode.h"

namespace te
{
    VideoMode::VideoMode(UINT32 width, UINT32 height, float refreshRate)
        : _width(width), _height(height), _refreshRate(refreshRate)
    {}

    VideoMode::~VideoMode()
    {}

    bool VideoMode::operator== (const VideoMode& other) const
    {
        return _width == other._width && _height == other._height && _refreshRate == other._refreshRate;
    }

    VideoOutputInfo::~VideoOutputInfo()
    {
        for (auto& videoMode : _videoModes)
        {
            te_delete(videoMode);
        }

        if (_desktopVideoMode != nullptr)
        {
            te_delete(_desktopVideoMode);
        }
    }

    VideoModeInfo::~VideoModeInfo()
    {
        for (auto& output : _outputs)
        {
            te_delete(output);
        }
    }
}