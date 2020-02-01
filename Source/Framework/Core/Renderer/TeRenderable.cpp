#include "TeRenderable.h"
#include "TeRenderer.h"
#include "Scene/TeSceneObject.h"

namespace te
{
    Renderable::Renderable()
        : _rendererId(0)
    { }

    Renderable::~Renderable()
    {
        if (_active)
            gRenderer()->NotifyRenderableRemoved(this);
    }

    void Renderable::Initialize()
    { 
        gRenderer()->NotifyRenderableAdded(this);
        CoreObject::Initialize();
    }

    void Renderable::_markCoreDirty(ActorDirtyFlag flag) 
    {
        MarkCoreDirty((UINT32)flag);
    }

    void Renderable::SetTransform(const Transform& transform)
    {
        if (_mobility != ObjectMobility::Movable)
            return;

        _transform = transform;
        _tfrmMatrix = transform.GetMatrix();
        _tfrmMatrixNoScale = Matrix4::TRS(transform.GetPosition(), transform.GetRotation(), Vector3::ONE);

        _markCoreDirty(ActorDirtyFlag::Transform);
    }

    void Renderable::SetMesh(HMesh& mesh)
    {
        _mesh = mesh;
        OnMeshChanged();
        _markCoreDirty();
    }

    Bounds Renderable::GetBounds() const
    {
        SPtr<Mesh> mesh = GetMesh().GetInternalPtr();

        if (mesh == nullptr)
        {
            const Transform& tfrm = GetTransform();

            AABox box(tfrm.GetPosition(), tfrm.GetPosition());
            Sphere sphere(tfrm.GetPosition(), 0.0f);

            return Bounds(box, sphere);
        }
        else
        {
            Bounds bounds = mesh->GetProperties().GetBounds();
            bounds.TransformAffine(_tfrmMatrix);

            return bounds;
        }
    }

    void Renderable::_updateState(const SceneObject& so, bool force)
    {
        UINT32 curHash = so.GetTransformHash();
        if (curHash != _hash || force)
        {
            SetTransform(so.GetTransform());

            _hash = curHash;
        }

        // Hash now matches so transform won't be applied twice, so we can just call base class version
        SceneActor::_updateState(so, force);
    }

    SPtr<Renderable> Renderable::Create()
    {
        SPtr<Renderable> handlerPtr = CreateEmpty();
        handlerPtr->Initialize();

        return handlerPtr;
    }

    SPtr<Renderable> Renderable::CreateEmpty()
    {
        Renderable* handler = new (te_allocate<Renderable>()) Renderable();
        SPtr<Renderable> handlerPtr = te_core_ptr<Renderable>(handler);
        handlerPtr->SetThisPtr(handlerPtr);

        return handlerPtr;
    }
}
