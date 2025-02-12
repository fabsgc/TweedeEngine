#pragma once

#include "TeCorePrerequisites.h"
#include "Components/TeCSoftBody.h"
#include "Physics/TeMeshSoftBody.h"

namespace te
{
    /**
     * @copydoc	MeshSoftBody
     *
     * @note Wraps MeshSoftBody as a Component.
     */
    class TE_CORE_EXPORT CMeshSoftBody : public CSoftBody
    {
    public:
        /** Return Component type */
        static CoreType GetComponentType() { return CoreType::TID_CMeshSoftBody; }

        /** @copydoc Component::Clone */
        bool Clone(const HComponent & c, const String & suffix = "");

        /** @copydoc Component::Clone */
        bool Clone(const HMeshSoftBody& c, const String& suffix = "");

        /** @copydoc MeshSoftBody::SetMesh */
        void SetMesh(const HPhysicsMesh& mesh);

        /** @copydoc MeshSoftBody::GetMesh */
        const HPhysicsMesh& GetMesh() const { return _mesh; }

    public:
        /** @copydoc Component::ExportJson */
        void ExportJson(nlohmann::json& document) const override;

    protected:
        friend class SceneObject;

        CMeshSoftBody(); // Serialization only
        explicit CMeshSoftBody(const HSceneObject& parent);

        /** @copydoc CBody::CreateInternal */
        SPtr<Body> CreateInternal() override;

        /** @copydoc Component::OnEnabled */
        void OnEnabled() override;

    protected:
        HPhysicsMesh _mesh;
    };
}
