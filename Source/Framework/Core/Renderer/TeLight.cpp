#include "TeLight.h"
#include "Renderer/TeRenderer.h"

namespace te
{
    Light::Light()
        : _type(LightType::Directional)
        , _castsShadows(false)
        , _color(Color::White)
        , _attRadius(10.0f)
        , _sourceRadius(0.0f)
        , _intensity(100.0f)
        , _spotAngle(45)
        , _spotFalloffAngle(35.0f)
        , _autoAttenuation(false)
        , _shadowBias(0.5f)
    { 
        UpdateAttenuationRange();
    }

    Light::Light(LightType type, Color color, float intensity, float attRadius, float srcRadius,
        bool castsShadows, Degree spotAngle, Degree spotFalloffAngle)
        : _type(type)
        , _castsShadows(castsShadows)
        , _color(color)
        , _attRadius(attRadius)
        , _sourceRadius(srcRadius)
        , _intensity(intensity)
        , _spotAngle(spotAngle)
        , _spotFalloffAngle(spotFalloffAngle)
        , _autoAttenuation(false)
        , _shadowBias(0.5f)
    { 
        UpdateAttenuationRange();
    }

    Light::~Light()
    {
        gRenderer()->NotifyLightRemoved(this);
    }

    void Light::Initialize()
    { 
        UpdateBounds();
        gRenderer()->NotifyLightAdded(const_cast<Light*>(this));

        CoreObject::Initialize();
    }

    void Light::_markCoreDirty(ActorDirtyFlag flag) 
    {
        MarkCoreDirty((UINT32)flag);
    }

    void Light::FrameSync()
    {
        TE_PRINT("# SYNC LIGHT");

        LightType oldType = _type;
        UINT32 dirtyFlag = GetCoreDirtyFlags();
        UINT32 updateEverythingFlag = (UINT32)ActorDirtyFlag::Everything;

        UpdateBounds();

        if ((dirtyFlag & updateEverythingFlag) != 0)
        {
            LightType newType = _type;
            _type = oldType;
            gRenderer()->NotifyLightRemoved(this);
            _type = newType;
            gRenderer()->NotifyLightAdded(this);
        }
        else if ((dirtyFlag & (UINT32)ActorDirtyFlag::Mobility) != 0)
        {
            gRenderer()->NotifyLightRemoved(this);
            gRenderer()->NotifyLightAdded(this);
        }
        else if ((dirtyFlag & (UINT32)ActorDirtyFlag::Transform) != 0)
        {
            gRenderer()->NotifyLightUpdated(this);
        }
    }

    void Light::SetMobility(ObjectMobility mobility)
    {
        SceneActor::SetMobility(mobility);
        _markCoreDirty(ActorDirtyFlag::Mobility);
    }

    void Light::SetTransform(const Transform& transform)
    {
        if (_mobility != ObjectMobility::Movable)
            return;

        SceneActor::SetTransform(transform);
        UpdateBounds();
    }

    void Light::SetUseAutoAttenuation(bool enabled)
    {
        _autoAttenuation = enabled;

        if (enabled)
            UpdateAttenuationRange();

        _markCoreDirty();
    }

    void Light::SetAttenuationRadius(float radius)
    {
        if (_autoAttenuation)
            return;

        _attRadius = radius;
        _markCoreDirty();
        UpdateBounds();
    }

    void Light::SetSourceRadius(float radius)
    {
        _sourceRadius = radius;

        if (_autoAttenuation)
            UpdateAttenuationRange();

        _markCoreDirty();
    }

    void Light::SetIntensity(float intensity)
    {
        _intensity = intensity;

        if (_autoAttenuation)
            UpdateAttenuationRange();

        _markCoreDirty();
    }

    float Light::GetLuminance() const
    {
        float radius2 = _sourceRadius * _sourceRadius;

        switch (_type)
        {
        case LightType::Radial:
            if (_sourceRadius > 0.0f)
                return _intensity / (4 * radius2 * Math::PI); // Luminous flux -> luminance
            else
                return _intensity / (4 * Math::PI); // Luminous flux -> luminous intensity
        case LightType::Spot:
        {
            if (_sourceRadius > 0.0f)
                return _intensity / (radius2 * Math::PI); // Luminous flux -> luminance
            else
            {
                // Note: Consider using the simpler conversion I / PI to match with the area-light conversion
                float cosTotalAngle = Math::Cos(_spotAngle);
                float cosFalloffAngle = Math::Cos(_spotFalloffAngle);

                // Luminous flux -> luminous intensity
                return _intensity / (Math::TWO_PI * (1.0f - (cosFalloffAngle + cosTotalAngle) * 0.5f));
            }
        }
        case LightType::Directional:
            if (_sourceRadius > 0.0f)
            {
                // Use cone solid angle formulae to calculate disc solid angle
                float solidAngle = Math::TWO_PI * (1 - cos(_sourceRadius * Math::DEG2RAD));
                return _intensity / solidAngle; // Illuminance -> luminance
            }
            else
                return _intensity; // In luminance units by default
        default:
            return 0.0f;
        }
    }

    void Light::UpdateAttenuationRange()
    {
        // Value to which intensity needs to drop in order for the light contribution to fade out to zero
        const float minAttenuation = 0.2f;

        if (_sourceRadius > 0.0f)
        {
            // Inverse of the attenuation formula for area lights:
            //   a = I / (1 + (2/r) * d + (1/r^2) * d^2
            // Where r is the source radius, and d is the distance from the light. As derived here:
            //   https://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/

            float luminousFlux = GetIntensity();

            float a = sqrt(minAttenuation);
            _attRadius = (_sourceRadius * (sqrt(luminousFlux - a))) / a;
        }
        else // Based on the basic inverse square distance formula
        {
            float luminousIntensity = GetIntensity();

            float a = minAttenuation;
            _attRadius = sqrt(std::max(0.0f, luminousIntensity / a));
        }

        UpdateBounds();
    }

    void Light::UpdateBounds()
    {
        const Transform& tfrm = GetTransform();

        switch (_type)
        {
        case LightType::Directional:
            _bounds = Sphere(tfrm.GetPosition(), std::numeric_limits<float>::infinity());
            break;
        case LightType::Radial:
            _bounds = Sphere(tfrm.GetPosition(), _attRadius);
            break;
        case LightType::Spot:
        {
            // Note: We could use the formula for calculating the circumcircle of a triangle (after projecting the cone),
            // but the radius of the sphere is the same as in the formula we use here, yet it is much simpler with no need
            // to calculate multiple determinants. Neither are good approximations when cone angle is wide.
            Vector3 offset(0, 0, _attRadius * 0.5f);

            // Direction along the edge of the cone, on the YZ plane (doesn't matter if we used XZ instead)
            Degree angle = Math::Clamp(_spotAngle * 0.5f, Degree(-89), Degree(89));
            Vector3 coneDir(0, Math::Tan(angle) * _attRadius, _attRadius);

            // Distance between the "corner" of the cone and our center, must be the radius (provided the center is at
            // the middle of the range)
            float radius = (offset - coneDir).Length();

            Vector3 center = tfrm.GetPosition() - tfrm.GetRotation().Rotate(offset);
            _bounds = Sphere(center, radius);
        }
        break;
        default:
            break;
        }
    }

    SPtr<Light> Light::Create(LightType type, Color color, float intensity, float attRadius, 
        bool castsShadows, Degree spotAngle, Degree spotFalloffAngle)
    {
        Light* handler = new (te_allocate<Light>())Light(type, color, intensity, attRadius, 0.0f, castsShadows, spotAngle, spotFalloffAngle);
        SPtr<Light> handlerPtr = te_core_ptr<Light>(handler);
        handlerPtr->SetThisPtr(handlerPtr);
        handlerPtr->Initialize();

        return handlerPtr;
    }
}
