#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "RenderAPI/TeVideoMode.h"
#include <X11/extensions/Xrandr.h>

namespace te
{
    /** @copydoc VideoMode */
    class LinuxVideoMode : public VideoMode
    {
    public:
        LinuxVideoMode(UINT32 width, UINT32 height, float refreshRate, UINT32 outputIdx);

        /** Returns internal RandR video mode id. */
        RRMode _getModeID() const { return mModeID; }

    private:
        LinuxVideoMode(UINT32 width, UINT32 height, float refreshRate, UINT32 outputIdx, RRMode modeID);
        friend class LinuxVideoOutputInfo;

        RRMode _modeID;
    };

    /** @copydoc VideoOutputInfo */
    class LinuxVideoOutputInfo : public VideoOutputInfo
    {
    public:
        LinuxVideoOutputInfo(::Display* x11Display, INT32 screen, XRROutputInfo* outputInfo, XRRCrtcInfo* crtcInfo,
             XRRScreenResources* screenRes, RROutput outputID, UINT32 outputIdx);

        /** Returns internal RandR output device id. */
        RROutput _getOutputID() const { return _outputID; }

        /** Returns X11 screen this output renders to. One screen can contain multiple output devices. */
        INT32 _getScreen() const { return _screen;}
    private:
        RROutput _outputID;
        INT32 _screen;
    };

    /** @copydoc VideoModeInfo */
    class LinuxVideoModeInfo : public VideoModeInfo
    {
    public:
        LinuxVideoModeInfo();
    };
}
