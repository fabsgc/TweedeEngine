#include "Components/TeCPatchSoftBody.h"

#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"

namespace te
{
    CPatchSoftBody::CPatchSoftBody()
        : CSoftBody(HSceneObject(), (UINT32)TID_CPatchSoftBody)
    {
        SetName("PatchSoftBody");
    }

    CPatchSoftBody::CPatchSoftBody(const HSceneObject& parent)
        : CSoftBody(parent, (UINT32)TID_CPatchSoftBody)
        , _topLeft(-0.5f, 0.0f, 0.5f)
        , _topRight(0.5f, 0.0f, 0.5f)
        , _bottomLeft(-0.5f, 0.0f, -0.5f)
        , _bottomRight(0.5f, 0.0f, -0.5f)
        , _resolutionX(8)
        , _resolutionY(8)
    {
        SetName("PatchSoftBody");
    }

    bool CPatchSoftBody::Clone(const HComponent& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        return Clone(static_object_cast<CPatchSoftBody>(c), suffix);
    }

    bool CPatchSoftBody::Clone(const HPatchSoftBody& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        if (CSoftBody::Clone(static_object_cast<CSoftBody>(c), suffix))
        {
            _topLeft = c->_topLeft;
            _topRight = c->_topRight;
            _bottomLeft = c->_bottomLeft;
            _bottomRight = c->_bottomRight;
            _resolutionX = c->_resolutionX;
            _resolutionY = c->_resolutionY;

            return true;
        }

        return false;
    }

    void CPatchSoftBody::SetCorners(const Vector3& topLeft, const Vector3& topRight, const Vector3& bottomLeft, const Vector3& bottomRight)
    {
        if (_topLeft == topLeft && _topRight == topRight && _bottomLeft == bottomLeft && _bottomRight == bottomRight)
            return;

        _topLeft = topLeft;
        _topRight = topRight;
        _bottomLeft = bottomLeft;
        _bottomRight = bottomRight;

        if (_internal != nullptr)
            std::static_pointer_cast<PatchSoftBody>(_internal)->SetCorners(topLeft, topRight, bottomLeft, bottomRight);
    }

    void CPatchSoftBody::GetCorners(Vector3& topLeft, Vector3& topRight, Vector3& bottomLeft, Vector3& bottomRight) const
    {
        topLeft = _topLeft;
        topRight = _topRight;
        bottomLeft = _bottomLeft;
        bottomRight = _bottomRight;
    }

    void CPatchSoftBody::SetResolution(UINT32 x, UINT32 y)
    {
        if (_resolutionX == x && _resolutionY == y)
            return;

        _resolutionX = x;
        _resolutionY = y;

        std::static_pointer_cast<PatchSoftBody>(_internal)->SetResolution(x, y);
    }

    void CPatchSoftBody::GetResolution(UINT32& x, UINT32& y) const
    {
        x = _resolutionX;
        y = _resolutionY;
    }

    void CPatchSoftBody::OnEnabled()
    {
        CSoftBody::OnEnabled();

        if (_internal)
        { 
            std::static_pointer_cast<PatchSoftBody>(_internal)->SetCorners(_topLeft, _topRight, _bottomLeft, _bottomRight);
            std::static_pointer_cast<PatchSoftBody>(_internal)->SetResolution(_resolutionX, _resolutionY);
        }
    }

    SPtr<Body> CPatchSoftBody::CreateInternal()
    {
        SPtr<PatchSoftBody> body = PatchSoftBody::Create(SO());
        body->SetOwner(PhysicsOwnerType::Component, this);

        return body;
    }
}
