#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    /**
	 * A base class for objects that can be placed in the scene. It has a transform object that allows it to be positioned,
	 * scaled and rotated, as well a properties that control its mobility (movable vs. immovable) and active status.
	 *
	 * In a way scene actors are similar to SceneObject%s, the main difference being that their implementations perform
	 * some functionality directly, rather than relying on attached Components. Scene actors can be considered as a
	 * lower-level alternative to SceneObject/Component model. In fact many Components internally just wrap scene actors.
	 */
    class TE_CORE_EXPORT SceneActor
    {
    public:
        SceneActor() = default;
        virtual ~SceneActor() = default;

        /**
		 * Determines if the actor is currently active. Deactivated actors act as if they have been destroyed, without
		 * actually being destroyed.
		 */
		virtual void SetActive(bool active);

		/** @copydoc setActive */
		bool GetActive() const { return _active; }

        /**
		 * Determines the mobility of the actor. This is used primarily as a performance hint to engine systems. Objects
		 * with more restricted mobility will result in higher performance. Any transform changes to immobile actors will
		 * be ignored. By default actor's mobility is unrestricted.
		 */
		virtual void SetMobility(ObjectMobility mobility);

		/** @copydoc setMobility */
		ObjectMobility GetMobility() const { return _mobility; }

    private:
        ObjectMobility _mobility = ObjectMobility::Movable;
		bool _active = true;
    };
}
