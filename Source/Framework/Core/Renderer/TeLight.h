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

        /**
         * Range at which the light contribution fades out to zero. Use setUseAutoAttenuation to provide a radius
         * automatically dependant on light intensity. The radius will cut-off light contribution and therefore manually set
         * very small radius can end up being very physically incorrect.
         */
        void SetAttenuationRadius(float radius);

        /**	@copydoc SetAttenuationRadius */
        float GetAttenuationRadius() const { return _attRadius; }

        /**
         * Radius of the light source. If non-zero then this light represents an area light, otherwise it is a punctual
         * light. Area lights have different attenuation then punctual lights, and their appearance in specular reflections
         * is realistic. Shape of the area light depends on light type:
         *  - For directional light the shape is a disc projected on the hemisphere on the sky. This parameter
         *    represents angular radius (in degrees) of the disk and should be very small (think of how much space the Sun
         *    takes on the sky - roughly 0.25 degree radius).
         *  - For radial light the shape is a sphere and the source radius is the radius of the sphere.
         *  - For spot lights the shape is a disc oriented in the direction of the spot light and the source radius is the
         *    radius of the disc.
         */
        void SetSourceRadius(float radius);

        /**	@copydoc SetSourceRadius */
        float GetSourceRadius() const { return _sourceRadius; }

        /**
         * If enabled the attenuation radius will automatically be controlled in order to provide reasonable light radius,
         * depending on its intensity.
         */
        void SetUseAutoAttenuation(bool enabled);

        /** @copydoc SetUseAutoAttenuation */
        bool GetUseAutoAttenuation() const { return _autoAttenuation; }

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

        /**
         * Determines the falloff angle covered by a spot light. Falloff angle determines at what point does light intensity
         * starts quadratically falling off as the angle approaches the total spot angle.
         */
        void SetSpotFalloffAngle(const Degree& spotFallofAngle)
        { _spotFalloffAngle = spotFallofAngle; _markCoreDirty(); UpdateBounds(); }

        /** @copydoc SetSpotFalloffAngle */
        Degree GetSpotFalloffAngle() const { return _spotFalloffAngle; }

        /**	Returns world space bounds that completely encompass the light's area of influence. */
        Sphere GetBounds() const { return _bounds; }

        /**
         * Returns the luminance of the light source. This is the value that should be used in lighting equations.
         *
         * @note
         * For point light sources this method returns luminous intensity and not luminance. We use the same method for both
         * as a convenience since in either case its used as a measure of intensity in lighting equations.
         */
        float GetLuminance() const;

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
            float intensity = 100.0f, float attRadius = 10.0f, bool castsShadows = false,
            Degree spotAngle = Degree(45), Degree spotFalloffAngle = Degree(40));

        /** @copydoc CoreObject::_markCoreDirty */
        void _markCoreDirty(ActorDirtyFlag flag = ActorDirtyFlag::Everything) override;

        /** @copydoc CoreObject::FrameSync */
        void FrameSync() override;

    protected:
        Light();
        Light(LightType type, Color color, float intensity, float attRadius, float srcRadius,
            bool castsShadows, Degree spotAngle, Degree spotFalloffAngle);

        /** Updates the internal bounds for the light. Call this whenever a property affecting the bounds changes. */
        void UpdateBounds();

        /** Calculates maximum light range based on light intensity. */
        void UpdateAttenuationRange();

        /** @copydoc SceneActor::SetTransform */
        void SetTransform(const Transform& transform) override;

    protected:
        LightType _type; /**< Type of light that determines how are the rest of the parameters interpreted. */
        bool _castsShadows; /**< Determines whether the light casts shadows. */
        Color _color; /**< Color of the light. */
        float _attRadius; /**< Radius at which light intensity falls off to zero. */
        float _sourceRadius; /**< Radius of the light source. If > 0 the light is an area light. */
        float _intensity; /**< Power of the light source. @see setIntensity. */
        Degree _spotAngle; /**< Total angle covered by a spot light. */
        Degree _spotFalloffAngle; /**< Spot light angle at which falloff starts. Must be smaller than total angle. */
        Sphere _bounds; /**< Sphere that bounds the light area of influence. */
        bool _autoAttenuation; /**< Determines is attenuation radius is automatically determined. */
        float _shadowBias; /**< See SetShadowBias() */
        UINT32 _rendererId;
    };
}
