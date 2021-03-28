#include "TeBulletRigidBody.h"
#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"
#include "TeBulletPhysics.h"

namespace te
{
    BulletRigidBody::BulletRigidBody(BulletPhysics* physics, BulletScene* scene, const HSceneObject& linkedSO)
        : RigidBody(linkedSO)
    {

    }

    BulletRigidBody::~BulletRigidBody()
    {

    }
}
