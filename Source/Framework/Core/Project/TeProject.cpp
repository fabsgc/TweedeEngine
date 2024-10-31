#include "Project/TeProject.h"

#include "Resources/TeResourceManager.h"
#include "ThirdParty/Slugify/slugify.hpp"
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
        _resourceNames.push_back(slugify(resource->GetName()) + ".resource");
    }

    void Project::Serialize(StreamWriter* serializer) const
    {
        Resource::Serialize(serializer);

        nlohmann::json sceneJsonDocument;

        serializer->WriteArray(_resourceNames);
        serializer->WriteString(sceneJsonDocument.dump());
    }

    void Project::Deserialize(StreamReader* deserializer, Project* object)
    {
        if (!object)
        {
            object = CreateEmpty().get();
        }

        Resource::Deserialize(deserializer, object);

        String sceneJsonString;

        deserializer->ReadArray(object->_resourceNames);
        deserializer->ReadString(sceneJsonString);

        nlohmann::json sceneJsonDocument = nlohmann::json::parse(sceneJsonString);
    }
}
