#include "Project/TeProject.h"

#include "Resources/TeResourceManager.h"
#include "ThirdParty/Slugify/slugify.hpp"

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
        _resourceTypes.push_back(resource->GetCoreType());
    }

    void Project::Serialize(StreamWriter* serializer) const
    {
        Resource::Serialize(serializer);

        serializer->WriteArray(_resourceNames);
        serializer->WriteArray(_resourceTypes);
    }

    void Project::Deserialize(StreamReader* deserializer, Project* object)
    {
        if (!object)
        {
            object = CreateEmpty().get();
        }

        Resource::Deserialize(deserializer, object);

        deserializer->ReadArray(object->_resourceNames);
        deserializer->ReadArray(object->_resourceTypes);
    }
}
