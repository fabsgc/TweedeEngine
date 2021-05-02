#pragma once

#include "TeCorePrerequisites.h"
#include "TeGuiAPI.h"

namespace te
{
    /** Factory class that you may specialize in order to start up a specific gui render system. */
    class GuiAPIFactory
    {
    public:
        virtual ~GuiAPIFactory() { }

        /** Creates and starts up the render system managed by this factory.  */
        virtual SPtr<GuiAPI> Create() = 0;

        /** Returns the name of the render system this factory creates. */
        virtual const String& Name() const = 0;
    };
}
