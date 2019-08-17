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
}