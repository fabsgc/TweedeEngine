#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    class TE_CORE_EXPORT Serializable
    {
    public:
        Serializable(CoreType type) { _coreType = type; }
        virtual ~Serializable() = default;

        CoreType GetCoreType() const { return _coreType; }

    protected:
        CoreType _coreType;
    };
}
