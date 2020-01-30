#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    class TE_CORE_EXPORT Serializable
    {
    public:
        Serializable(UINT32 type) { _coreType = type; }
        ~Serializable() = default;

        UINT32 GetCoreType() const { return _coreType; }
    
    protected:
        UINT32 _coreType;
    };
}
