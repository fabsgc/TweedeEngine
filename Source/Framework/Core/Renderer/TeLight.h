#pragma once

#include "TeCorePrerequisites.h"
#include "CoreUtility/TeCoreObject.h"
#include "Scene/TeSceneActor.h"
#include "Math/TeSphere.h"
#include "Image/TeColor.h"

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

        /**	Determines the type of the light. */
        LightType GetType() const { return _type; }

        /** @copydoc getType() */
        void SetType(LightType type) { _type = type; _markCoreDirty(); UpdateBounds(); }

        /**	Determines does this light cast shadows when rendered. */
        void SetCastsShadow(bool castsShadow) { _castsShadows = castsShadow; _markCoreDirty(); }

        /** @copydoc SetCastsShadow */
        bool GetCastsShadow() const { return _castsShadows; }

        /**
         * Shadow bias determines offset at which the shadows are rendered from the shadow caster. Bias value of 0 means
         * the shadow will be renderered exactly at the casters position. If your geometry has thin areas this will
         * produce an artifact called shadow acne, in which case you can increase the shadow bias value to eliminate it.
         * Note that increasing the shadow bias will on the other hand make the shadow be offset from the caster and may
         * make the caster appear as if floating (Peter Panning artifact). Neither is perfect, so it is preferable to ensure
         * all your geometry has thickness and keep the bias at zero, or even at negative values.
         *
         * Default value is 0.5. Should be in rough range [-1, 1].
         */
        void SetShadowBias(float bias) { _shadowBias = bias; _markCoreDirty(); }

        /** @copydoc SetShadowBias() */
        float GetShadowBias() const { return _shadowBias; }

        /** Determines the color emitted by the light. */
        void SetColor(const Color& color) { _color = color; _markCoreDirty(); }

        /** @copydoc SetColor() */
        Color GetColor() const { return _color; }

        /** Range at which the light contribution fades out to zero. */
        void SetAttenuationRadius(float radius);

        /**	@copydoc SetAttenuationRadius */
        float GetAttenuationRadius() const { return _attRadius; }

        /** Range at which the light contribution fades out to zero. */
        void SetLinearAttenuation(float attenuation);

        /**	@copydoc SetAttenuationRadius */
        float GetLinearAttenuation() const { return _linearAttenuation; }

        /** Range at which the light contribution fades out to zero. */
        void SetQuadraticAttenuation(float attenuation);

        /**	@copydoc SetAttenuationRadius */
        float GetQuadraticAttenuation() const { return _quadraticAttenuation; }

        /**
         * Determines the power of the light source. This will be luminous flux for radial & spot lights,
         * luminance for directional lights with no area, and illuminance for directional lights with area (non-zero source
         * radius).
         */
        void SetIntensity(float intensity);

        /** @copydoc SetIntensity */
        float GetIntensity() const { return _intensity; }

        /**	Determines the total angle covered by a spot light. */
        void SetSpotAngle(const Degree& spotAngle) { _spotAngle = spotAngle; _markCoreDirty(); UpdateBounds(); }

        /** @copydoc SetSpotAngle */
        Degree GetSpotAngle() const { return _spotAngle; }

        /**	Returns world space bounds that completely encompass the light's area of influence. */
        Sphere GetBounds() const { return _bounds; }

        /**
         * Creates a new light with provided settings.
         *
         * @param[in]	type				Type of light that determines how are the rest of the parameters interpreted.
         * @param[in]	color				Color of the light.
         * @param[in]	intensity			Power of the light source. This will be luminous flux for radial & spot lights,
         *									luminance for directional lights with no area, and illuminance for directional
         *									lights with area (non-zero source radius).
         * @param[in]	attRadius			Radius at which light's influence fades out to zero.
         * @param[in]	castsShadows		Determines whether the light casts shadows.
         * @param[in]	spotAngle			Total angle covered by a spot light.
         * @param[in]	spotFalloffAngle	Spot light angle at which falloff starts. Must be smaller than total angle.
         */
        static SPtr<Light> Create(LightType type = LightType::Directional, Color color = Color::White,
            float intensity = DefaultIntensity, float attRadius = DefaultAttRadius, float linearAtt = DefaultLinearAtt, 
            float quadraticAtt = DefaultQuadraticAtt, bool castsShadows = DefaultCastShadow,
            Degree spotAngle = Degree(DefaultSpotAngle));

        /** @copydoc CoreObject::_markCoreDirty */
        void _markCoreDirty(ActorDirtyFlag flag = ActorDirtyFlag::Everything) override;

        /** @copydoc CoreObject::FrameSync */
        void FrameSync() override;

    public:
        static bool DefaultCastShadow;
        static float DefaultAttRadius;
        static float DefaultLinearAtt;
        static float DefaultQuadraticAtt;
        static float DefaultIntensity;
        static float DefaultSpotAngle;
        static float DefaultShadowBias;

    protected:
        Light();
        Light(LightType type, Color color, float intensity, float attRadius, float linearAtt, 
            float quadraticAtt, bool castsShadows, Degree spotAngle);

        /** Updates the internal bounds for the light. Call this whenever a property affecting the bounds changes. */
        void UpdateBounds();

        /** @copydoc SceneActor::SetTransform */
        void SetTransform(const Transform& transform) override;

    protected:
        LightType _type; /**< Type of light that determines how are the rest of the parameters interpreted. */
        bool _castsShadows; /**< Determines whether the light casts shadows. */
        Color _color; /**< Color of the light. */
        float _attRadius; /**< Radius at which light intensity falls off to zero. */
        float _linearAttenuation; /**< Radius at which light intensity falls off to zero. */
        float _quadraticAttenuation; /**< Radius at which light intensity falls off to zero. */
        float _intensity; /**< Power of the light source. @see SetIntensity. */
        Degree _spotAngle; /**< Total angle covered by a spot light. */
        Sphere _bounds; /**< Sphere that bounds the light area of influence. */
        float _shadowBias; /**< See SetShadowBias() */
        UINT32 _rendererId;
    };
}
