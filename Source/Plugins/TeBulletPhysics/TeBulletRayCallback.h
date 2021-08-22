#pragma once

#include "TeBulletPhysicsPrerequisites.h"

namespace te
{
    struct BulletClosestRayResultCallback : public btCollisionWorld::ClosestRayResultCallback
    {
        BulletClosestRayResultCallback(const btVector3& rayFromWorld, const btVector3& rayToWorld)
            : btCollisionWorld::ClosestRayResultCallback(rayFromWorld, rayToWorld)
        {
            m_localShapeInfo.m_shapePart = -1;
            m_localShapeInfo.m_triangleIndex = -1;
        }

        virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
        {
            m_localShapeInfo = *rayResult.m_localShapeInfo;
            return btCollisionWorld::ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
        }

        btCollisionWorld::LocalShapeInfo m_localShapeInfo;
    };

    struct BulletAllHitsRayResultCallback : public btCollisionWorld::AllHitsRayResultCallback
    {
        BulletAllHitsRayResultCallback(const btVector3& rayFromWorld, const btVector3& rayToWorld)
            : btCollisionWorld::AllHitsRayResultCallback(rayFromWorld, rayToWorld)
        {
        }

        virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
        {
            m_localShapeInfos.push_back(*rayResult.m_localShapeInfo);
            return btCollisionWorld::AllHitsRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
        }

        Vector<btCollisionWorld::LocalShapeInfo> m_localShapeInfos;
    };
}
