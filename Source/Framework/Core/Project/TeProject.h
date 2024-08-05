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
        virtual ~Project() = default;

        /**  @copydoc Resource::GetResourceType */
        static UINT32 GetResourceType() { return TypeID_Core::TID_Project; }

        /**	Creates a new project from the provided per-size font data. */
        static HProject Create();

        /** Creates a new project as a pointer instead of a resource handle. */
        static SPtr<Project> CreatePtr();

        /** Creates a Project without initializing it. */
        static SPtr<Project> CreateEmpty();

        /** A project can store all resources currently used */
        Vector<Resource*> GetAllResources() const { return _resources; }

        /** @copydoc GetAllResources  */
        void AddResource(Resource* resource) { _resources.push_back(resource); }

        /** @copydoc GetAllResources  */
        void ClearResources() { _resources.clear(); }

    public:
        void Serialize(Serializer* serialize) const override;

    private:
        Project();

    private:
        Vector<Resource*> _resources;
    };
}