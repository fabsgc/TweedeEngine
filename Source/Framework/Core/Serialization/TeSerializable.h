#pragma once

#include "TeCorePrerequisites.h"

#include "Serialization/TeStreamWriter.h"
#include "Serialization/TeStreamReader.h"

namespace te
{
    class TE_CORE_EXPORT Serializable
    {
    public:
        Serializable(UINT32 type) { _coreType = type; }
        virtual ~Serializable() = default;

        UINT32 GetCoreType() const { return _coreType; }

        virtual void Serialize(StreamWriter* serializer) const
        {
            serializer->WriteRaw<UINT32>(_coreType);
        }

        static void Deserialize(StreamReader* deserialize, Serializable* object)
        {
            if (!object)
                return;

            deserialize->ReadRaw<UINT32>(object->_coreType);
        }

    protected:
        UINT32 _coreType;
    };
}
