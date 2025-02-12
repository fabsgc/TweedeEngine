#pragma once

#include "TeCorePrerequisites.h"

#include "Utility/TeNonCopyable.h"
#include "CoreUtility/TeCoreObject.h"
#include "Serialization/TeSerializable.h"
#include "Serialization/TeStreamWriter.h"
#include "Serialization/TeStreamReader.h"

namespace te
{
    /** Base class for all resources. */
    class TE_CORE_EXPORT Resource : public CoreObject, public Serializable, public NonCopyable
    {
    public:
        virtual ~Resource() = default;

        /** Returns the name of the resource. */
        virtual const String& GetName() const { return _name; }

        /** Sets the name of the resource. */
        virtual void SetName(const String& name) { _name = name; }

        /** Returns the path of the resource. */
        virtual const String& GetPath() const { return _path; };

        /** Sets the path of the resource. */
        virtual void SetPath(const String& path) { _path = path; }

        /**	Globally unique identifier of the resource that persists scene save/load. */
        virtual const UUID& GetUUID() const { return _UUID; }

        /** Allow to dynamically retrieve resource type */
        static CoreType GetResourceType() { return CoreType::TID_Resource; }

    public:
        /** @copydoc Serializable::Serialize */
        virtual void Serialize(StreamWriter* serializer) const;

        /** @copydoc Serializable::Deserialize */
        static void Deserialize(StreamReader* deserializer, Resource* object);

    protected:
        friend class ResourceManager;
        friend class ResourceHandleBase;
        friend class ProjectImporter;

        Resource(CoreType type);

        /** @copydoc GetUUID */
        virtual void SetUUID(const UUID& uuid) { _UUID = uuid; }

    protected:
        String _name;
        String _path;
        UINT32 _size;
        UUID   _UUID;
    };
}
