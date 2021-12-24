#include "Physics/TeSoftBody.h"
#include "Physics/TePhysics.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"
#include "TeFSoftBody.h"

namespace te
{ 
    SoftBody::SoftBody(const HSceneObject& linkedSO)
        : Body(linkedSO, TypeID_Core::TID_SoftBody)
    { }

    void SoftBody::SetScale(const Vector3& scale)
    {
        static_cast<FSoftBody*>(_internal)->SetScale(scale);
    }

    const Vector3& SoftBody::GetScale() const
    {
        return static_cast<FSoftBody*>(_internal)->GetScale();
    }
}
