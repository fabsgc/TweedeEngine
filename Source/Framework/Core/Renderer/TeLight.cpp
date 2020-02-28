#include "TeLight.h"
#include "Renderer/TeRenderer.h"

namespace te
{
    Light::Light()
        : _type(LightType::Directional)
        , _castsShadows(false)
        , _color(Color::White)
        , _attRadius(10.0f)
        , _intensity(0.2f)
        , _spotAngle(45)
        , _shadowBias(0.5f)
    { }

    Light::Light(LightType type, Color color, float intensity, float attRadius, float srcRadius,
        bool castsShadows, Degree spotAngle, Degree spotFalloffAngle)
        : _type(type)
        , _castsShadows(castsShadows)
        , _color(color)
        , _attRadius(attRadius)
        , _intensity(intensity)
        , _spotAngle(spotAngle)
        , _shadowBias(0.5f)
    { }

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
        // TE_PRINT("# SYNC LIGHT");

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

    void Light::SetAttenuationRadius(float radius)
    {
        _attRadius = radius;
        _markCoreDirty();
        UpdateBounds();
    }

    void Light::SetIntensity(float intensity)
    {
        _intensity = intensity;
        _markCoreDirty();
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
