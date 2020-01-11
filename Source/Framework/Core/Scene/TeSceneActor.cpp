#include "TeSceneActor.h"

namespace te
{
    void SceneActor::SetMobility(ObjectMobility mobility)
	{
		_mobility = mobility;
	}

	void SceneActor::SetActive(bool active)
	{
		_active = active;
	}
}