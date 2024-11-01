#pragma once

#include "TeCorePrerequisites.h"

#include "Resources/TeResource.h"
#include "ThirdParty/Slugify/slugify.hpp"

#include <filesystem>

namespace te::serialization
{
    static const String ProjectResourceExtension = ".resource";

    inline String GetResourceName(const Resource* resource)
    {
        return slugify(resource->GetUUID().ToString() + "_" + resource->GetName());
    }

    inline std::filesystem::path GetResourcepath(const std::filesystem::path& workingDirectory, const String& name)
    {
        std::filesystem::path resourcePath = workingDirectory;
        resourcePath += std::filesystem::path::preferred_separator;
        resourcePath += name;
        resourcePath += ProjectResourceExtension;

        return resourcePath;
    }
    
    inline std::filesystem::path GetProjectResourcesPath(const std::filesystem::path& workingDirectory)
    {
        std::filesystem::path resourcesPath = workingDirectory;
        resourcesPath += std::filesystem::path::preferred_separator;
        resourcesPath += "resources";

        return resourcesPath;
    }

    inline std::filesystem::path GetProjectResourcePath(const std::filesystem::path& workingDirectory, const Resource* resource)
    {
        std::filesystem::path resourcePath = GetResourcepath(GetProjectResourcesPath(workingDirectory), GetResourceName(resource));
        
        return resourcePath;
    }

    inline std::filesystem::path GetProjectResourcePath(const std::filesystem::path& workingDirectory, const String& name)
    {
        std::filesystem::path resourcePath = GetResourcepath(GetProjectResourcesPath(workingDirectory), name);
        
        return resourcePath;
    }
}
