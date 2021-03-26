#include "Win32/TeWin32VideoModeInfo.h"
#include "Math/TeMath.h"

namespace te
{
    BOOL CALLBACK MonitorEnumCallback(HMONITOR hMonitor, HDC hdc, LPRECT rect, LPARAM lParam)
    {
        Vector<HMONITOR>* outputInfos = (Vector<HMONITOR>*)lParam;
        outputInfos->push_back(hMonitor);

        return TRUE;
    };

    Win32VideoModeInfo::Win32VideoModeInfo()
    {
        Vector<HMONITOR> handles;
        EnumDisplayMonitors(0, nullptr, &MonitorEnumCallback, (LPARAM)&handles);

        // Sort so that primary is the first output
        for (auto iter = handles.begin(); iter != handles.end(); ++iter)
        {
            MONITORINFOEX monitorInfo;

            memset(&monitorInfo, 0, sizeof(MONITORINFOEX));
            monitorInfo.cbSize = sizeof(MONITORINFOEX);
            GetMonitorInfo(*iter, &monitorInfo);

            if ((monitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0)
            {
                if (iter != handles.begin())
                {
                    HMONITOR temp = handles[0];
                    handles[0] = *iter;
                    *iter = temp;
                }

                break;
            }
        }

        UINT32 idx = 0;
        for (auto& handle : handles)
        {
            _outputs.push_back(te_new<Win32VideoOutputInfo>(handle, idx++));
        }
    }

    Win32VideoOutputInfo::Win32VideoOutputInfo(HMONITOR monitorHandle, UINT32 outputIdx)
        : _monitorHandle(monitorHandle)
    {
        MONITORINFOEX monitorInfo;

        memset(&monitorInfo, 0, sizeof(MONITORINFOEX));
        monitorInfo.cbSize = sizeof(MONITORINFOEX);
        GetMonitorInfo(_monitorHandle, &monitorInfo);

        _name = monitorInfo.szDevice;

        DEVMODE devMode;
        devMode.dmSize = sizeof(DEVMODE);
        devMode.dmDriverExtra = 0;

        UINT32 i = 0;
        while (EnumDisplaySettings(monitorInfo.szDevice, i++, &devMode))
        {
            bool foundVideoMode = false;
            for (auto videoMode : _videoModes)
            {
                Win32VideoMode* win32VideoMode = static_cast<Win32VideoMode*>(videoMode);

                UINT32 intRefresh = Math::RoundToInt(win32VideoMode->_refreshRate);
                if (win32VideoMode->_width == devMode.dmPelsWidth && win32VideoMode->_height == devMode.dmPelsHeight
                    && intRefresh == devMode.dmDisplayFrequency)
                {
                    foundVideoMode = true;
                    break;
                }
            }

            if (!foundVideoMode)
            {
                Win32VideoMode* videoMode = te_new<Win32VideoMode>(devMode.dmPelsWidth, devMode.dmPelsHeight,
                    (float)devMode.dmDisplayFrequency, outputIdx);

                _videoModes.push_back(videoMode);
            }
        }

        // Get desktop display mode
        EnumDisplaySettings(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode);

        Win32VideoMode* desktopVideoMode = te_new<Win32VideoMode>(devMode.dmPelsWidth, devMode.dmPelsHeight,
            (float)devMode.dmDisplayFrequency, outputIdx);

        _desktopVideoMode = desktopVideoMode;
    }

    Win32VideoMode::Win32VideoMode(UINT32 width, UINT32 height, float refreshRate, UINT32 outputIdx)
        : VideoMode(width, height, refreshRate, outputIdx)
    { }
}
