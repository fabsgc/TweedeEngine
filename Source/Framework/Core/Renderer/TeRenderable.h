#pragma once

#include "TeCorePrerequisites.h"
#include "CoreUtility/TeCoreObject.h"
#include "Scene/TeSceneActor.h"

namespace te
{
    /** Illuminates a portion of the scene covered by the Renderable. */
    class TE_CORE_EXPORT Renderable : public CoreObject, public SceneActor
    {
    public:
        Renderable() = default;
        ~Renderable() = default;

        /** @copydoc CoreObject::Initialize */
        void Initialize() override;

        /** @copydoc CoreObject::Destroy */
        void Destroy() override;

        /** @copydoc CoreObject::_markCoreDirty */
        void _markCoreDirty(ActorDirtyFlag flag = ActorDirtyFlag::Everything) override;

        /** @copydoc SceneActor::SetTransform */
        void SetTransform(const Transform& transform) override;
    };
}
