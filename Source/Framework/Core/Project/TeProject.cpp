#include "Project/TeProject.h"

#include "Resources/TeResourceManager.h"
#include "ThirdParty/Slugify/slugify.hpp"
#include "Serialization/TeUtility.h"
#include "Scene/TeSceneObject.h"
#include "Json/json.h"

namespace te
{
    Project::Project()
        : Resource(TID_Project)
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
        _resourceNames.push_back(serialization::GetResourceName(resource));
    }

    void Project::Serialize(StreamWriter* serializer) const
    {
        Resource::Serialize(serializer);

        nlohmann::json projectJsonDocument;
        
        projectJsonDocument["resources"] = _resourceNames;
        projectJsonDocument["scene"].push_back(nlohmann::json());

        _sceneObject->ExportJson(projectJsonDocument["scene"].back());

        String dump = projectJsonDocument.dump();
        serializer->WriteString(dump);
    }

    void Project::Deserialize(StreamReader* deserializer, Project* object)
    {
        if (!object)
        {
            object = CreateEmpty().get();
        }

        Resource::Deserialize(deserializer, object);

        String projectJsonString;
        deserializer->ReadString(projectJsonString);

        nlohmann::json projectJsonDocument = nlohmann::json::parse(projectJsonString);

        for (auto& resource : projectJsonDocument["resources"])
        {
            object->_resourceNames.push_back(resource.get<String>());
        }

        // Should I load resources here ?

        if (projectJsonDocument.contains("scene") && projectJsonDocument["scene"].size() > 0)
        {
            object->_sceneObject = SceneObject::Create(projectJsonDocument["scene"][0]["name"].get<String>());
            object->_sceneObject->SetUUID(UUID(projectJsonDocument["scene"][0]["uuid"].get<String>()));
        }
    }
}
