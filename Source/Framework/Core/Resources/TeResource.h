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
        virtual ~Resource() = default;

        /** Returns the name of the resource. */
        const String& GetName() const { return _name; }

        /** Sets the name of the resource. */
        void SetName(const String& name) { _name = name; }

        /** Returns the path of the resource. */
        const String& GetPath() const { return _path; };

        /** Sets the path of the resource. */
        void SetPath(const String& path) { _path = path; }

        /**	Globally unique identifier of the resource that persists scene save/load. */
        const UUID& GetUUID() const { return _UUID; }

        /** @copydoc GetUUID */
        void _setUUID(const UUID& uuid) { _UUID = uuid; }

        /** Allow to dynamically retrieve resource type */
        static UINT32 GetResourceType() { return TypeID_Core::TID_Resource; }

    protected:
        friend class ResourceManager;
        friend class ResourceHandleBase;

        Resource(UINT32 type);

    protected:
        String _name;
        String _path;
        UINT32 _size;
        UUID   _UUID;
    };
}
