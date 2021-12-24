#include "Physics/TeSoftBody.h"
#include "Physics/TePhysics.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{ 
    SoftBody::SoftBody(const HSceneObject& linkedSO)
        : Body(linkedSO, TypeID_Core::TID_SoftBody)
    { }
}
