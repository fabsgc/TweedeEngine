#include "Project/TeProject.h"

#include "Importer/TeResourceImportOptions.h"
#include "ThirdParty/Slugify/slugify.hpp"
#include "Resources/TeResourceManager.h"
#include "Serialization/TeBinaryReader.h"
#include "Serialization/TeUtility.h"
#include "Scene/TeSceneObject.h"
#include "Json/json.h"

namespace te
{
    Project::Project()
        : Resource(CoreType::TID_Project)
    { }

    HProject Project::Create()
    {
        SPtr<Project> newProject = CreatePtr();
        return static_resource_cast<Project>(gResourceManager()._createResourceHandle(newProject));
    }

    SPtr<Project> Project::CreatePtr()
    {
        SPtr<Project> newProject = te_core_ptr<Project>(new (te_allocate<Project>()) Project());
        newProject->SetThisPtr(newProject);
        newProject->Initialize();

        return newProject;
    }

    SPtr<Project> Project::CreateEmpty()
    {
        SPtr<Project> newProject = te_core_ptr<Project>(new (te_allocate<Project>()) Project());
        newProject->SetThisPtr(newProject);

        return newProject;
    }

    void Project::AddResource(Resource* resource)
    { 
        _resources.push_back(resource);
    }

    void Project::Serialize(StreamWriter* serializer) const
    {
        Resource::Serialize(serializer);

        nlohmann::json document;

        auto resources = _resources;

        std::sort(resources.begin(), resources.end(),
            [](const Resource* a, const Resource* b) {
                return a->GetCoreType() < b->GetCoreType();
            });

        for (const auto& resource : resources)
        {
            document["resources"].push_back(serialization::GetResourceName(resource));
        }

        document["scene"].push_back(nlohmann::json());

        _sceneObject->ExportJson(document["scene"].back());

        String dump = document.dump();
        serializer->WriteString(dump);
    }

    bool Project::Deserialize(StreamReader* deserializer, Project* object, const std::filesystem::path& workingDirectory)
    {
        if (!object)
            return false;

        Resource::Deserialize(deserializer, object);

        Resource* resourceMetaData = new Resource(CoreType::TID_Resource);
        String documentStr;

        deserializer->ReadString(documentStr);
        nlohmann::json document = nlohmann::json::parse(documentStr);

        for (const auto& resource : document["resources"])
        {
            const std::filesystem::path resourcePath = serialization::GetProjectResourcePath(workingDirectory, resource.get<String>());
            if (!std::filesystem::exists(resourcePath))
            {
                TE_DEBUG("Resource with path \"" + resourcePath.generic_string() + "\" does not exist.");
                continue;
            }

            BinaryReader* resourceDeserializer = te_new<BinaryReader>(resourcePath);
            if (Resource::Deserialize(resourceDeserializer, resourceMetaData))
            {
                ResourceImportOptions importOptions;
                importOptions.ResourceType = resourceMetaData->GetCoreType();

                HResource res = gResourceManager().Load<Resource>(resourcePath.generic_string(), importOptions);
                if (res.IsLoaded())
                {
                    object->AddResource(res.Get());
                    TE_DEBUG("Resource imported from the specified path : " + res->GetPath().generic_string());
                }
                else
                {
                    TE_DEBUG("Failed to import the resource from the specified path : " + resourcePath.generic_string());
                }
            }
            else
            {
                TE_DEBUG("Failed to deserialize the resource meta data from the specified path : " + resourcePath.generic_string());
            }

            te_delete(resourceDeserializer);
        }

        resourceMetaData->Destroy();
        te_delete(resourceMetaData);

        if (document.contains("scene") && document["scene"].size() == 1)
        {
            HSceneObject parent;
            object->_sceneObject = SceneObject::ImportJson(parent, document["scene"][0]);
        }

        object->Initialize();

        return true;
    }
}
