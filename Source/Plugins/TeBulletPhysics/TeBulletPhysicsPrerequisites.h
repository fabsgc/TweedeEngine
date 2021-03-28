#pragma once

#include "TeCorePrerequisites.h"

#include "Math/TeVector3.h"
#include "Math/TeVector4.h"
#include "Math/TeQuaternion.h"

#pragma warning(push, 0) 
#include "LinearMath/btQuaternion.h"

#include "btBulletDynamicsCommon.h"
#include "BulletCollision/BroadphaseCollision/btDbvtBroadphase.h"
#include "BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h"
#include "BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h"
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "BulletCollision/CollisionShapes/btCollisionShape.h"
#include "BulletSoftBody/btSoftBody.h"
#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"
#include "BulletSoftBody/btSoftBodyHelpers.h"
#include "BulletCollision/CollisionShapes/btSphereShape.h"
#include "BulletCollision/CollisionShapes/btCylinderShape.h"
#include "BulletCollision/CollisionShapes/btCapsuleShape.h"
#include "BulletCollision/CollisionShapes/btStaticPlaneShape.h"
#include "BulletCollision/CollisionShapes/btConeShape.h"
#include "BulletCollision/CollisionShapes/btConvexHullShape.h"
#include "BulletDynamics/ConstraintSolver/btHingeConstraint.h"
#include "BulletDynamics/ConstraintSolver/btSliderConstraint.h"
#include "BulletDynamics/ConstraintSolver/btConeTwistConstraint.h"
#include "BulletDynamics/ConstraintSolver/btPoint2PointConstraint.h"
#pragma warning(pop)

namespace te
{
    class BulletPhysics;

    inline Vector3 ToVector3(const btVector3& vector)
    {
        return Vector3(vector.getX(), vector.getY(), vector.getZ());
    }

    inline Vector4 ToVector4(const btVector3& vector)
    {
        return Vector4(vector.getX(), vector.getY(), vector.getZ(), 1.0f);
    }

    inline btVector3 ToBtVector3(const Vector3& vector)
    {
        return btVector3(vector.x, vector.y, vector.z);
    }

    inline btQuaternion ToBtQuaternion(const Quaternion& quaternion)
    {
        return btQuaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
    }

    inline Quaternion ToQuaternion(const btQuaternion& quaternion)
    {
        return Quaternion(quaternion.getX(), quaternion.getY(), quaternion.getZ(), quaternion.getW());
    }
}
