#pragma once

#include "TeCorePrerequisites.h"
#include "ThirdParty/Json/json.h"

namespace te::serialization
{
    class JsonSerialization
    {
    public:
        virtual void ExportJson(nlohmann::json& document) const = 0;
    };
}
