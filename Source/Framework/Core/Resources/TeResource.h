#pragma once

#include "TeCorePrerequisites.h"
#include "CoreUtility/TeCoreObject.h"
#include "Utility/TeNonCopyable.h"
#include "Serialization/TeSerializable.h"

namespace te
{
    /** Base class for all resources. */
    class TE_CORE_EXPORT Resource : public CoreObject, public Serializable, public NonCopyable
    {
    public:
        Resource(UINT32 type);
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
        UINT32 _size = 0;
    };
}
