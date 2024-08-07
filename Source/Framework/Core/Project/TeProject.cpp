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

    void Project::Serialize(StreamWriter* serializer) const
    {
        Resource::Serialize(serializer);

        Vector<String> resources;

        for (auto& resource : _resources)
        {
            resources.push_back(slugify(resource->GetName()) + ".resource");
        }

        serializer->WriteArray(resources);
    }

    void Project::Deserialize(StreamReader* deserializer, Project* object)
    {
        if (!object)
        {
            object = CreateEmpty().get();
        }

        Resource::Deserialize(deserializer, object);

        Vector<String> resources;
        deserializer->ReadArray(resources);
    }
}
