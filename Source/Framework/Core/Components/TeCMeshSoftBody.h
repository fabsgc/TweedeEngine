#pragma once

#include "TeCorePrerequisites.h"
#include "Components/TeCSoftBody.h"
#include "Physics/TeMeshSoftBody.h"
#include "Physics/TePhysicsMesh.h"

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
        static UINT32 GetComponentType() { return TypeID_Core::TID_CMeshSoftBody; }

        /** @copydoc Component::Initialize */
        void Initialize() override;

        /** @copydoc Component::Update */
        void Update() override;

        /** @copydoc Component::Clone */
        bool Clone(const HComponent & c, const String & suffix = "");

        /** @copydoc Component::Clone */
        bool Clone(const HMeshSoftBody& c, const String& suffix = "");

        /** @copydoc MeshSoftBody::SetMesh */
        void SetMesh(const HPhysicsMesh& mesh);

        /** @copydoc MeshSoftBody::GetMesh */
        HPhysicsMesh GetMesh() const { return _mesh; }

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
