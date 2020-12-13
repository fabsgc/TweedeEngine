#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    /** Types of notifications we would like to receive when we start a FolderMonitor on a certain folder. */
    enum class FolderChangeBit
    {
        FileName 	= 1 << 0, /**< Called when file is created, moved or removed. */
        DirName 	= 1 << 1, /**< Called when directory is created, moved or removed. */
        FileWrite 	= 1 << 2, /**< Called when file is written to. */
    };

    /**
     * Allows monitoring a file system folder for changes. Depending on the flags set this monitor can notify you when file
     * is changed/moved/renamed and similar.
     */
    class TE_CORE_EXPORT FolderMonitor
    {
    };
}
