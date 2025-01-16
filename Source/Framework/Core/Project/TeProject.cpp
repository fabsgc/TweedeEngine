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

        nlohmann::json document;
        
        document["resources"] = _resourceNames;
        document["scene"].push_back(nlohmann::json());

        _sceneObject->ExportJson(document["scene"].back());

        String dump = document.dump();
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

        nlohmann::json document = nlohmann::json::parse(projectJsonString);

        for (auto& resource : document["resources"])
        {
            object->_resourceNames.push_back(resource.get<String>());
        }

        // Should I load resources here ? Yes, but I can also load them directly within the deserialize methods of those that need them.
        // I should write a API to load a resource given a path RessourceMananger::Load<T>(const Project&, const String& name)

        if (document.contains("scene") && document["scene"].size() > 0)
        {
            object->_sceneObject = SceneObject::Create(document["scene"][0]["name"].get<String>());
            object->_sceneObject->SetUUID(UUID(document["scene"][0]["uuid"].get<String>()));
        }
    }
}
