#pragma once

#include "TeCorePrerequisites.h"
#include "CoreUtility/TeCoreObject.h"
#include "Utility/TeNonCopyable.h"

namespace te
{
    /** Base class for all resources. */
    class TE_CORE_EXPORT Resource : public CoreObject, public NonCopyable
    {
    public:
        Resource() {};
        virtual ~Resource() = default;

        /** Returns the name of the resource. */
        const String& GetName() const;

        /** Sets the name of the resource.  */
        void SetName(const String& name);

    protected:
        friend class ResourceManager;
        friend class ResourceHandleBase;

    protected:
        String _name;
        UINT32 _size;
    };
}
