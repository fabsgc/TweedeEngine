#include "TeLight.h"
#include "Renderer/TeRenderer.h"

namespace te
{
    bool Light::DefaultCastShadows = false;
    float Light::DefaultIntensity = 500.f;
    float Light::DefaultSpotAngle = 45.f;
    float Light::DefaultShadowBias = 0.5f;
    Light::CastShadowsType Light::DefaultCastShadowsType = Light::CastShadowsType::Both;

    const UINT32 Light::LIGHT_CONE_NUM_SIDES = 20;
    const UINT32 Light::LIGHT_CONE_NUM_SLICES = 10;

    Light::Light()
        : Serializable(TID_Light)
        , _type(Light::Type::Directional)
        , _castShadows(DefaultCastShadows)
        , _color(Color::White)
        , _intensity(DefaultIntensity)
        , _spotAngle(DefaultSpotAngle)
        , _shadowBias(DefaultShadowBias)
        , _castShadowsType(DefaultCastShadowsType)
    { }

    Light::Light(Light::Type type, Color color, float intensity, 
        bool castShadows, Light::CastShadowsType castShadowsType, Degree spotAngle)
        : Serializable(TID_Light)
        , _type(type)
        , _castShadows(castShadows)
        , _color(color)
        , _intensity(intensity)
        , _spotAngle(spotAngle)
        , _shadowBias(DefaultShadowBias)
        , _castShadowsType(castShadowsType)
    { }

    Light::~Light()
    {
        if (_renderer) _renderer->NotifyLightRemoved(this);
    }

    void Light::Initialize()
    {
        UpdateBounds();
        if (_renderer) _renderer->NotifyLightAdded(const_cast<Light*>(this));

        CoreObject::Initialize();
    }

    void Light::_markCoreDirty(ActorDirtyFlag flag) 
    {
        MarkCoreDirty((UINT32)flag);
    }

    void Light::FrameSync()
    {
        Light::Type oldType = _type;
        UINT32 dirtyFlag = GetCoreDirtyFlags();
        UINT32 updateEverythingFlag = (UINT32)ActorDirtyFlag::Everything | (UINT32)ActorDirtyFlag::Active;

        UpdateBounds();

        if ((dirtyFlag & updateEverythingFlag) != 0)
        {
            if (_oldActive != _active)
            {
                if (_renderer) 
                {
                    if (_active)
                    {
                        _renderer->NotifyLightAdded(this);
                    }
                    else
                    {
                        Light::Type newType = _type;
                        _type = oldType;
                        _renderer->NotifyLightRemoved(this);
                        _type = newType;
                    }
                }
            }
            else if (_renderer) 
            {
                Light::Type newType = _type;
                _type = oldType;
                _renderer->NotifyLightRemoved(this);
                _type = newType;
                _renderer->NotifyLightAdded(this);
            }
        }
        else if ((dirtyFlag & (UINT32)ActorDirtyFlag::Mobility) != 0)
        {
            // TODO I'm not sure for that, we might check if SceneActor is active
            if (_active && _renderer)
            {
                _renderer->NotifyLightRemoved(this);
                _renderer->NotifyLightAdded(this);
            }
        }
        else if ((dirtyFlag & (UINT32)ActorDirtyFlag::Transform) != 0 ||
                 (dirtyFlag & (UINT32)LightDirtyFlag::RedrawShadow) != 0)
        {
            if (_active && _renderer)
                _renderer->NotifyLightUpdated(this, dirtyFlag);
        }

        _oldActive = _active;
    }

    void Light::AttachTo(SPtr<Renderer> renderer)
    {
        if (_renderer)
            _renderer->NotifyLightRemoved(this);

        _renderer = renderer;

        if (_renderer)
            _renderer->NotifyLightAdded(this);

        _markCoreDirty();
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
    void Light::SetIntensity(float intensity)
    {
        _intensity = intensity;
        _markCoreDirty();
    }

    void Light::UpdateBounds()
    {
        const Transform& tfrm = GetTransform();

        // We assume that below 0.0001f, attenuation is equal to zero
        // Attenuation is calculated using this formula : Att =  1 / (4 * PI * D²)
        // We want D when Att = 0.0001f
        // D = sqrt ( 1 / (4 * PI * Att ))
        float maxRadius = 1.f;

        if (_type == Light::Type::Radial || _type == Light::Type::Spot)
        {
            float minAtt = 0.0001f;
            maxRadius = Math::Sqrt(1.f / ( 4 * Math::PI * minAtt ));
        }

        switch (_type)
        {
        case Light::Type::Directional:
            _bounds = Sphere(tfrm.GetPosition(), std::numeric_limits<float>::infinity());
            break;
        case Light::Type::Radial:
            _bounds = Sphere(tfrm.GetPosition(), maxRadius);
            break;
        case Light::Type::Spot:
        {
            // Note: We could use the formula for calculating the circumcircle of a triangle (after projecting the cone),
            // but the radius of the sphere is the same as in the formula we use here, yet it is much simpler with no need
            // to calculate multiple determinants. Neither are good approximations when cone angle is wide.
            Vector3 offset(0, 0, maxRadius * 0.5f);

            // Direction along the edge of the cone, on the YZ plane (doesn't matter if we used XZ instead)
            Degree angle = Math::Clamp(_spotAngle * 0.5f, Degree(-89), Degree(89));
            Vector3 coneDir(0, Math::Tan(angle) * maxRadius, maxRadius);

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

    SPtr<Light> Light::Create(Light::Type type, Color color, float intensity, bool castShadows, 
        Light::CastShadowsType castShadowsType, Degree spotAngle)
    {
        Light* handler = new (te_allocate<Light>()) Light(type, color, intensity, castShadows, castShadowsType, spotAngle);
        SPtr<Light> handlerPtr = te_core_ptr<Light>(handler);
        handlerPtr->SetThisPtr(handlerPtr);
        handlerPtr->Initialize();

        return handlerPtr;
    }
}
