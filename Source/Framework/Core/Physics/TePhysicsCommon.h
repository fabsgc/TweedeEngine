#pragma once

#include "TeCorePrerequisites.h"
#include "Math/TeVector3.h"

namespace te
{ 
    /** Information about a single contact point during physics collision. */
    struct ContactPoint
    {
        Vector3 Position; /**< Contact point in world space. */
        Vector3 Normal; /**< Normal pointing from the second shape to the first shape. */
        /** Impulse applied to the objects to keep them from penetrating. Divide by simulation step to get the force. */
        float Impulse;
        float Separation; /**< Determines how far are the objects. Negative value denotes penetration. */
    };

    /** Information about a collision between two physics objects. */
    struct CollisionDataRaw
    {
        Body* Bodies[2]; /**< Bodies involved in the collision. */

        // Note: Not too happy this is heap allocated, use static allocator?
        Vector<ContactPoint> ContactPoints; /**< Information about all the contact points for the hit. */
    };

    /** Information about a collision between two physics objects. */
    struct CollisionData
    {
        /** Components of the bodies that have collided. */
        HBody Bodies[2];

        // Note: Not too happy this is heap allocated, use static allocator?
        Vector<ContactPoint> ContactPoints; /**< Information about all the contact points for the hit. */
    };

    /** Determines what parent, if any, owns a physics object. */
    enum class PhysicsOwnerType
    {
        None, /** No parent, object is used directly. */
        Component, /** Object is used by a C++ Component. */
    };

    /** Contains information about a parent for a physics object. */
    struct PhysicsObjectOwner
    {
        PhysicsOwnerType Type = PhysicsOwnerType::None; /**< Type of owner. */
        void* OwnerData = nullptr; /**< Data managed by the owner. */
    };

    /** Determines which collision events will be reported by physics objects. */
    enum class CollisionReportMode
    {
        None, /**< No collision events will be triggered. */
        Report, /**< Collision events will be triggered when object enters and/or leaves collision. */
        /**
         * Collision events will be triggered when object enters and/or leaves collision, but also every frame the object
         * remains in collision.
         */
        ReportPersistent,
    };
}
