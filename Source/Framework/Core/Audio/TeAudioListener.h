#pragma once

#include "TeCorePrerequisites.h"
#include "CoreUtility/TeCoreObject.h"
#include "Scene/TeSceneActor.h"
#include "Math/TeVector3.h"

namespace te
{
     /**
      * Represents a listener that hears audio sources. For spatial audio the volume and pitch of played audio is determined
      * by the distance, orientation and velocity differences between the source and the listener.
      */
    class TE_CORE_EXPORT AudioListener : public CoreObject, public SceneActor
    {
    public:
        virtual ~AudioListener() = default;

        /** Sets the velocity of the listener. */
        virtual void SetVelocity(const Vector3& velocity);

        /** Retrieves the velocity of the listener. */
        Vector3 GetVelocity() const { return _velocity; }

        /** Creates a new audio listener. */
        static SPtr<AudioListener> Create();

    protected:
        AudioListener();

        /** @copydoc CoreObject::Initialize */
        virtual void Initialize() override;

    protected:
        Vector3 _velocity = TeZero;
    };
}
