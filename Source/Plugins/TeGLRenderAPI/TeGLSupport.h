#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "TeGLRenderAPI.h"
#include "RenderAPI/TeRenderWindow.h"

namespace te
{
    /**
     * Helper class dealing mostly with platform specific OpenGL functionality, initialization, extensions and window
     * creation.
     */
    class GLSupport
    {
    public:
        GLSupport() = default;
        virtual ~GLSupport() = default;

        /**	Called when OpenGL is being initialized. */
        virtual void Start() = 0;

        /**	Called when OpenGL is being shut down. */
        virtual void Stop() = 0;

        /**	Gets OpenGL vendor name. */
        const String& GetGLVendor() const
        {
            return _vendor;
        }

        /**	Gets OpenGL version string. */
        const String& GetGLVersion() const
        {
            return _version;
        }

        /**	Checks is the specified extension available. */
        virtual bool CheckExtension(const String& ext) const;

        /**	Gets an address of an OpenGL procedure with the specified name. */
        virtual void* GetProcAddress(const String& procname) = 0;

        /** Initializes OpenGL extensions. Must be called after we have a valid and active OpenGL context. */
        virtual void InitializeExtensions();

        /**	Gets a structure describing all available video modes. */
        virtual SPtr<VideoModeInfo> GetVideoModeInfo() const = 0;

    protected:
        Vector<String> _extensionList;

        String _version;
        String _vendor;
    };
}
