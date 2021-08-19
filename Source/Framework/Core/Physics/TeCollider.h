#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TePhysicsCommon.h"
#include "Math/TeVector3.h"
#include "Utility/TeEvent.h"
#include "Math/TeQuaternion.h"

namespace te
{
    class Ray;

    /**
     * Collider represents part of physics geometries that can be in multiple states:
     *  - Static geometry that physics objects can collide with.
     *  - Static geometry that can't be collided with but will report touch events.
     */
    class TE_CORE_EXPORT Collider
    {
    public:
        Collider() = default;
        virtual ~Collider() = default;

        /** Determines the Rigidbody that controls this collider (if any). */
        void SetBody(Body* value);

        /** @copydoc Collider::SetBody */
        Body* GetBody() const { return _body; }

        /** Sets the scale of the collider geometry. */
        virtual void SetScale(const Vector3& scale);

        /** Retrieves the scale of the collider geometry. */
        Vector3 GetScale() const;

        /** @copydoc FCollider::SetCenter */
        void SetCenter(const Vector3& center);

        /** @copydoc FCollider::GetCenter */
        const Vector3& GetCenter() const;

        /** @copydoc FCollider::SetPosition */
        void SetPosition(const Vector3& position);

        /** @copydoc FCollider::GetPosition */
        const Vector3& GetPosition() const;

        /** @copydoc FCollider::SetPosition */
        void SetRotation(const Quaternion& rotation);

        /** @copydoc FCollider::GetPosition */
        const Quaternion& GetRotation() const;

        /**
         * Checks does the ray hit this collider.
         *
         * @param[in]	physicsScene	PhysicsScene where you want to cast your ray
         * @param[in]	ray		        Ray to check.
         * @param[out]	hit		        Information about the hit. Valid only if the method returns true.
         * @param[in]	maxDist	        Maximum distance from the ray origin to search for hits.
         * @return				        True if the ray has hit the collider.
         */
        bool RayCast(const SPtr<PhysicsScene>& physicsScene, const Ray& ray, PhysicsQueryHit& hit, float maxDist = FLT_MAX) const;

        /**
         * Checks does the ray hit this collider.
         *
         * @param[in]	physicsScene	PhysicsScene where you want to cast your ray
         * @param[in]	origin	        Origin of the ray to check.
         * @param[in]	unitDir	        Unit direction of the ray to check.
         * @param[out]	hit		        Information about the hit. Valid only if the method returns true.
         * @param[in]	maxDist	        Maximum distance from the ray origin to search for hits.
         * @return				        True if the ray has hit the collider.
         */
        bool RayCast(const SPtr<PhysicsScene>& physicsScene, const Vector3& origin, const Vector3& unitDir, PhysicsQueryHit& hit,
            float maxDist = FLT_MAX) const;

        /**
         * Checks does the ray hit this collider.
         *
         * @param[in]	physicsScene	PhysicsScene where you want to cast your ray
         * @param[in]	ray		        Ray to check.
         * @param[out]	hit		        Information about all the hits. Valid only if the method returns true.
         * @param[in]	maxDist	        Maximum distance from the ray origin to search for hits.
         * @return				        True if the ray has hit the collider.
         */
        bool RayCast(const SPtr<PhysicsScene>& physicsScene, const Ray& ray, Vector<PhysicsQueryHit>& hit, float maxDist = FLT_MAX) const;

        /**
         * Checks does the ray hit this collider.
         *
         * @param[in]	physicsScene	PhysicsScene where you want to cast your ray
         * @param[in]	origin	        Origin of the ray to check.
         * @param[in]	unitDir	        Unit direction of the ray to check.
         * @param[out]	hit		        Information about all the hits. Valid only if the method returns true.
         * @param[in]	maxDist	        Maximum distance from the ray origin to search for hits.
         * @return				        True if the ray has hit the collider.
         */
        bool RayCast(const SPtr<PhysicsScene>& physicsScene, const Vector3& origin, const Vector3& unitDir, Vector <PhysicsQueryHit>& hit,
            float maxDist = FLT_MAX) const;

        /** Returns the object containing common collider code. */
        FCollider* GetInternal() const { return _internal; }

        /**
         * Sets the object that owns this physics object, if any. Used for high level systems so they can easily map their
         * high level physics objects from the low level ones returned by various queries and events.
         */
        void SetOwner(PhysicsOwnerType type, void* owner) { _owner.Type = type; _owner.OwnerData = owner; }

        /**
         * Gets the object that owns this physics object, if any. Used for high level systems so they can easily map their
         * high level physics objects from the low level ones returned by various queries and events.
         */
        void* GetOwner(PhysicsOwnerType type) const { return _owner.Type == type ? _owner.OwnerData : nullptr; }

    protected:
        FCollider* _internal = nullptr;
        PhysicsObjectOwner _owner;
        Body* _body = nullptr;
    };
}
