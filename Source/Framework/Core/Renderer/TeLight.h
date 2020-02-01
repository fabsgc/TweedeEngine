#pragma once

#include "TeCorePrerequisites.h"
#include "CoreUtility/TeCoreObject.h"
#include "Scene/TeSceneActor.h"
#include "Math/TeSphere.h"

namespace te
{
    enum class LightType
    {
        Directional,
        Radial,
        Spot,
        Count // Keep at end
    };

    /** Illuminates a portion of the scene covered by the light. */
    class TE_CORE_EXPORT Light : public CoreObject, public SceneActor
    {
    public:
        ~Light();

        /** @copydoc CoreObject::Initialize */
        void Initialize() override;

        /** @copydoc SceneActor::Destroy */
        void SetMobility(ObjectMobility mobility) override;

        /**	Sets an ID that can be used for uniquely identifying this object by the renderer. */
        void SetRendererId(UINT32 id) { _rendererId = id; }

        /**	Retrieves an ID that can be used for uniquely identifying this object by the renderer. */
        UINT32 GetRendererId() const { return _rendererId; }

        /** @copydoc CoreObject::_markCoreDirty */
        void _markCoreDirty(ActorDirtyFlag flag = ActorDirtyFlag::Everything) override;

        /**	Determines the type of the light. */
        LightType GetType() const { return _type; }

        /** @copydoc getType() */
        void SetType(LightType type) { _type = type; _markCoreDirty(); UpdateBounds(); }

        /**	Returns world space bounds that completely encompass the light's area of influence. */
        Sphere GetBounds() const { return _bounds; }

        /**
         * Creates a new light with provided settings.
         *
         * @param[in]	type				Type of light that determines how are the rest of the parameters interpreted.
         */
        static SPtr<Light> Create(LightType type = LightType::Directional);

    protected:
        Light();
        Light(LightType type);

        /** Updates the internal bounds for the light. Call this whenever a property affecting the bounds changes. */
        void UpdateBounds();

        /** @copydoc SceneActor::SetTransform */
        void SetTransform(const Transform& transform) override;

    protected:
        LightType _type; /**< Type of light that determines how are the rest of the parameters interpreted. */
        Sphere _bounds; /**< Sphere that bounds the light area of influence. */
        UINT32 _rendererId;
    };
}
