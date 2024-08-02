#pragma once

#include "TeCorePrerequisites.h"

#include "TeSerializer.h"

namespace te
{
    class TE_CORE_EXPORT Serializable
    {
    public:
        Serializable(UINT32 type) { _coreType = type; }
        virtual ~Serializable() = default;

        UINT32 GetCoreType() const { return _coreType; }

        virtual void Serialize(Serializer& serializer) { }

        // TODO Serialization : Unserialize

    protected:
        UINT32 _coreType;
    };
}
