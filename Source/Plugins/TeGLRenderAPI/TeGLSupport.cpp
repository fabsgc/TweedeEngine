//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "TeGLSupport.h"
#include "TeGLTexture.h"
#include "GL/glew.h"

GLenum glewContextInit(te::GLSupport* glSupport);

namespace te
{
    void GLSupport::InitializeExtensions()
    {
        glewContextInit(this);
        TE_CHECK_GL_ERROR();

        // Set version string
        const GLubyte* pcVer = glGetString(GL_VERSION);
        assert(pcVer && "Problems getting GL version string using glGetString");

        String tmpStr = (const char*)pcVer;
        _version = tmpStr.substr(0, tmpStr.find(" "));

        // Get vendor
        const GLubyte* pcVendor = glGetString(GL_VENDOR);
        tmpStr = (const char*)pcVendor;
        _vendor = tmpStr.substr(0, tmpStr.find(" "));

        // Set extension list
        INT32 numExtensions = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
        TE_CHECK_GL_ERROR();

        for (INT32 i = 0; i < numExtensions; i++)
            _extensionList.push_back(String((char*)glGetStringi(GL_EXTENSIONS, i)));
    }

    bool GLSupport::CheckExtension(const String& ext) const
    {
        if(std::find(_extensionList.begin(), _extensionList.end(), ext) == _extensionList.end())
            return false;

        return true;
    }
}
