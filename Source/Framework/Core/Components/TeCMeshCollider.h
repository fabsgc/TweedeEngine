#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeMeshCollider.h"
#include "Components/TeCCollider.h"

namespace te
{
    /**
     * @copydoc	MeshCollider
     *
     * @note	Wraps MeshCollider as a Component.
     */
    class TE_CORE_EXPORT CMeshCollider : public CCollider
    {
    public:
        CMeshCollider(const HSceneObject& parent);

        /** Return Component type */
        static UINT32 GetComponentType() { return TID_CMeshCollider; }

        /** @copydoc Component::Clone */
        void Clone(const HMeshCollider& c);

    protected:
        friend class SceneObject;

        /** @copydoc CCollider::CreateInternal */
        SPtr<Collider> CreateInternal() override;

        /** @copydoc CCollider::RestoreInternal */
        void RestoreInternal() override;

        /**	Returns the mesh collider that this component wraps. */
        MeshCollider* _getInternal() const { return static_cast<MeshCollider*>(_internal.get()); }

    protected:
        CMeshCollider(); // Serialization only
    };
}
