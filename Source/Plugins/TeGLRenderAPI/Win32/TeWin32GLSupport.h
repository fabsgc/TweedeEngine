#pragma once

#include "Win32/TeWin32Prerequisites.h"
#include "TeGLSupport.h"
#include "TeGLRenderAPI.h"

namespace te
{
    /**	Handles OpenGL initialization, window creation and extensions on Windows. */
    class Win32GLSupport : public GLSupport
    {
    public:
        Win32GLSupport();

        /** @copydoc GLSupport::Start */
        void Start() override;

        /** @copydoc GLSupport::Stop */
        void Stop() override;

        /** @copydoc GLSupport::GetProcAddress */
        void* GetProcAddress(const String& procname) override;

        /** @copydoc GLSupport::InitializeExtensions */
        void InitializeExtensions() override;
        
        /**
         * Creates a new OpenGL context.
         *
         * @param[in]	hdc				Handle to device context to create the context from.
         * @param[in]	externalGlrc	(Optional) Handle to external OpenGL context. If not provided new context will be
         *								created.
         * @return						Newly created GLContext class referencing the created or external context handle.
         */
        SPtr<Win32Context> CreateContext(HDC hdc, HGLRC externalGlrc = 0);

        /**
         * Selects and sets an appropriate pixel format based on the provided parameters.
         *
         * @param[in]	hdc			Handle to device context to create the context from.
         * @param[in]	colorDepth	Wanted color depth of the pixel format, in bits.
         * @param[in]	multisample	Amount of multisampling wanted, if any.
         * @param[in]	hwGamma		Should the format support automatic gamma conversion on write/read.
         * @param[in]	depth		Should the pixel format contain the depth/stencil buffer.
         * @return					True if a pixel format was successfully set.
         */
        bool SelectPixelFormat(HDC hdc, int colorDepth, int multisample, bool hwGamma, bool depth);

        /** @copydoc GLSupport::GetVideoModeInfo */
        SPtr<VideoModeInfo> GetVideoModeInfo() const override;

        /** Notifies the manager that a new window has been created. */
        void NotifyWindowCreated(Win32RenderWindow* window);

    private:
        /**	Initializes windows specific OpenGL extensions needed for advanced context creation. */
        void InitialiseWGL();

        /**	Dummy window procedure used when creating the initial dummy OpenGL context. */
        static LRESULT CALLBACK DummyWndProc(HWND hwnd, UINT umsg, WPARAM wp, LPARAM lp);

        Vector<DEVMODE>    _devModes;
        Win32RenderWindow* _initialWindow = nullptr;
        Vector<int>        _multisampleLevels;
        bool               _hasPixelFormatARB = false;
        bool               _hasMultisample = false;
        bool               _hasHardwareGamma = false;
        bool               _hasAdvancedContext = false;
    };
}
