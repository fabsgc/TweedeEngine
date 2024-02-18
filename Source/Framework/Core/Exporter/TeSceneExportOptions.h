#pragma once

#include "TeCorePrerequisites.h"
#include "Exporter/TeExportOptions.h"

namespace te
{
    /** Contains export options you may use to control how is a scene exported. */
    class TE_CORE_EXPORT SceneExportOptions : public ExportOptions
    {
    public:
        SceneExportOptions();

        /**
         * Determines if we export a scene or just a generic group of objects. The only difference between the
         * two is the way root object is handled: scenes are assumed to be saved with the scene root object (which is
         * hidden), while object group root is a normal scene object (not hidden). This is relevant when scene are
         * loaded, so the systems knows to append the root object to non-root scenes.
         */
        bool isScene = false;

        /** Creates a new export options object that allows you to customize how are scenes exported. */
        static SPtr<SceneExportOptions> Create();
    };
}
