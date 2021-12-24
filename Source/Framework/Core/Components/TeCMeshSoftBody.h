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

    protected:
        friend class SceneObject;

        CMeshSoftBody(); // Serialization only
        CMeshSoftBody(const HSceneObject& parent);

        /** @copydoc CBody::CreateInternal */
        SPtr<Body> CreateInternal() override;
    };
}
