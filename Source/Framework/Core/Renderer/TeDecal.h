#pragma once

#include "TeCorePrerequisites.h"
#include "CoreUtility/TeCoreObject.h"
#include "Scene/TeSceneActor.h"
#include "Math/TeVector2.h"
#include "Math/TeBounds.h"

namespace te
{
    /**
     * Specifies a decal that will be projected onto scene geometry. User can set the material to use when rendering
     * the decal, as well as control decal orientation and size.
     */
    class TE_CORE_EXPORT Decal : public CoreObject, public SceneActor
    {
    public:
        ~Decal();

        /** @copydoc CoreObject::Initialize */
        void Initialize() override;

        /**	Sets an ID that can be used for uniquely identifying this object by the renderer. */
        void SetRendererId(UINT32 id) { _rendererId = id; }

        /**	Retrieves an ID that can be used for uniquely identifying this object by the renderer. */
        UINT32 GetRendererId() const { return _rendererId; }

        /**
         * Creates a new decal.
         *
         * @param[in]	material		Material to use when rendering the decal.
         * @param[in]	size			Size of the decal in world units.
         * @param[in]	maxDistance		Maximum distance at which will the decal be visible (from the current decal origin,
         *								along the negative Z axis).
         * @returns						New decal object.
         */
        static SPtr<Decal> Create(const HMaterial& material, const Vector2& size = Vector2::ONE, float maxDistance = 10.0f);

        /** @copydoc SceneActor::_markCoreDirty */
        void _markCoreDirty(ActorDirtyFlag flag = ActorDirtyFlag::Everything) override;

        /** @copydoc CoreObject::FrameSync */
        void FrameSync() override;

    protected:
        friend class CDecal;

        Decal();

    protected:
        UINT32 _rendererId;
    };
}
