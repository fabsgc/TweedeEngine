#include "TeSceneActor.h"

namespace te
{
    void SceneActor::SetTransform(const Transform& transform)
    {
        if (_mobility != ObjectMobility::Movable)
        {
            return;
        }

        _transform = transform;
    }

    void SceneActor::SetMobility(ObjectMobility mobility)
    {
        _mobility = mobility;
    }

    void SceneActor::SetActive(bool active)
    {
        _active = active;
    }
}