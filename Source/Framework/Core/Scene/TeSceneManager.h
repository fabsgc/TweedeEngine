#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    /**
	 * Keeps track of all active SceneObject%s and their components. Keeps track of component state and triggers their
	 * events. Updates the transforms of objects as SceneObject%s move.
	 */
	class TE_CORE_EXPORT SceneManager : public Module<SceneManager>
	{
	public:
        SceneManager();
        ~SceneManager();

        void Initialize();

        /**	Notifies the scene manager that a new camera was created. */
		void RegisterCamera(const SPtr<Camera>& camera);

		/**	Notifies the scene manager that a camera was removed. */
		void UnregisterCamera(const SPtr<Camera>& camera);

    protected:
        UnorderedMap<Camera*, SPtr<Camera>> _cameras;
    };

    /**	Provides easy access to the SceneManager. */
	TE_CORE_EXPORT SceneManager& gSceneManager();
}
