#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"

namespace te
{
    /**
     * Project resource. This object is the big box handling and managing all resources needed for a project
     */
    class TE_CORE_EXPORT Project : public Resource
    {
    public:
        /**  @copydoc Resource::GetResourceType */
        static CoreType GetResourceType() { return CoreType::TID_Project; }

        /**	Creates a new project from the provided per-size font data. */
        static HProject Create();

        /** Creates a new project as a pointer instead of a resource handle. */
        static SPtr<Project> CreatePtr();

        /** Creates a Project without initializing it (for serialization). */
        static SPtr<Project> CreateEmpty();

        /** A project can store a set of currently used resources */
        const Vector<Resource*>& GetAllResources() const { return _resources; }

        /** Add a resource to the project */
        void AddResource(Resource* resource);

        /** Add the scene to save */
        void SetSceneObject(HSceneObject& so) { _sceneObject = so; }

        /** @copydoc SetScene */
        const HSceneObject& GetSceneObject() const { return _sceneObject; }

        /** Remove all resources from the project */
        void ClearResources() { _resources.clear(); }

    public:
        void Serialize(StreamWriter* serializer) const override;

        static bool Deserialize(StreamReader* deserializer, Project* object, const std::filesystem::path& workingDirectory);

    private:
        Project();

    private:
        Vector<Resource*> _resources;
        HSceneObject _sceneObject;
    };
}