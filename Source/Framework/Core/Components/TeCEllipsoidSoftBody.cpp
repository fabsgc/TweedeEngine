#include "Components/TeCEllipsoidSoftBody.h"

#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"

namespace te
{
    CEllipsoidSoftBody::CEllipsoidSoftBody()
        : CSoftBody(HSceneObject(), (UINT32)TID_CEllipsoidSoftBody)
    {
        SetName("EllipsoidSoftBody");
    }

    CEllipsoidSoftBody::CEllipsoidSoftBody(const HSceneObject& parent)
        : CSoftBody(parent, (UINT32)TID_CEllipsoidSoftBody)
    {
        SetName("EllipsoidSoftBody");
    }

    bool CEllipsoidSoftBody::Clone(const HComponent& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        return Clone(static_object_cast<CEllipsoidSoftBody>(c), suffix);
    }

    bool CEllipsoidSoftBody::Clone(const HEllipsoidSoftBody& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        if (CSoftBody::Clone(static_object_cast<CSoftBody>(c), suffix))
        {
            _center = c->_center;
            _radius = c->_radius;
            _resolution = c->_resolution;

            return true;
        }

        return false;
    }

    void CEllipsoidSoftBody::SetCenter(const Vector3& center)
    {
        if (_center == center)
            return;

        _center = center;

        std::static_pointer_cast<EllipsoidSoftBody>(_internal)->SetCenter(center);
    }

    void CEllipsoidSoftBody::SetRadius(const Vector3& radius)
    {
        if (_radius == radius)
            return;

        _radius = radius;

        std::static_pointer_cast<EllipsoidSoftBody>(_internal)->SetRadius(radius);
    }

    void CEllipsoidSoftBody::SetResolution(UINT32 resolution)
    {
        if (_resolution == resolution)
            return;

        _resolution = resolution;

        std::static_pointer_cast<EllipsoidSoftBody>(_internal)->SetResolution(resolution);
    }

    void CEllipsoidSoftBody::OnEnabled()
    {
        CSoftBody::OnEnabled();

        if (_internal)
        { 
            std::static_pointer_cast<EllipsoidSoftBody>(_internal)->SetCenter(_center);
            std::static_pointer_cast<EllipsoidSoftBody>(_internal)->SetRadius(_radius);
            std::static_pointer_cast<EllipsoidSoftBody>(_internal)->SetResolution(_resolution);
        }
    }

    SPtr<Body> CEllipsoidSoftBody::CreateInternal()
    {
        SPtr<EllipsoidSoftBody> body = EllipsoidSoftBody::Create(SO());
        body->SetOwner(PhysicsOwnerType::Component, this);

        return body;
    }
}
