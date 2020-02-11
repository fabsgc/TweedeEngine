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
        gRenderer()->NotifyRenderableRemoved(this);
    }

    void Renderable::Initialize()
    { 
        gRenderer()->NotifyRenderableAdded(this);
        CoreObject::Initialize();
    }

    void Renderable::OnMeshChanged()
    {
        _markCoreDirty();
    }

    void Renderable::_markCoreDirty(ActorDirtyFlag flag) 
    {
        MarkCoreDirty((UINT32)flag);
    }

    void Renderable::FrameSync()
    {
        // TE_PRINT("# SYNC RENDERABLE");

        UINT32 dirtyFlag = GetCoreDirtyFlags();
        UINT32 updateEverythingFlag = (UINT32)ActorDirtyFlag::Everything;

        if ((dirtyFlag & updateEverythingFlag) != 0)
        {
            gRenderer()->NotifyRenderableRemoved(this);
            gRenderer()->NotifyRenderableAdded(this);
        }
        else if ((dirtyFlag & (UINT32)ActorDirtyFlag::Mobility) != 0)
        {
            gRenderer()->NotifyRenderableRemoved(this);
            gRenderer()->NotifyRenderableAdded(this);
        }
        else if ((dirtyFlag & (UINT32)ActorDirtyFlag::Transform) != 0)
        {
            gRenderer()->NotifyRenderableUpdated(this);
        }
        else if ((dirtyFlag & (UINT32)ActorDirtyFlag::GpuParams) != 0)
        {
            gRenderer()->NotifyRenderableUpdated(this);
        }
    }

    void Renderable::SetMobility(ObjectMobility mobility)
    {
        SceneActor::SetMobility(mobility);
        _markCoreDirty(ActorDirtyFlag::Mobility);
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

    void Renderable::SetMesh(SPtr<Mesh> mesh)
    {
        _mesh = mesh;

        UINT32 numSubMeshes = mesh->GetProperties().GetNumSubMeshes();
        _materials.resize(numSubMeshes);

        OnMeshChanged();
        _markCoreDirty(ActorDirtyFlag::GpuParams);
    }

    void Renderable::SetMaterial(UINT32 idx, const SPtr<Material>& material)
    {
        if (idx >= (UINT32)_materials.size())
            return;

        _materials[idx] = material;
        _markCoreDirty(ActorDirtyFlag::GpuParams);
    }

    void Renderable::SetMaterials(const Vector<SPtr<Material>>& materials)
    {
        _numMaterials = (UINT32)_materials.size();
        UINT32 min = std::min(_numMaterials, (UINT32)materials.size());

        for (UINT32 i = 0; i < min; i++)
            _materials[i] = materials[i];

        for (UINT32 i = min; i < _numMaterials; i++)
            _materials[i] = nullptr;

        _markCoreDirty(ActorDirtyFlag::GpuParams);
    }

    void Renderable::SetMaterial(const SPtr<Material>& material)
    {
        SetMaterial(0, material);
        _markCoreDirty(ActorDirtyFlag::GpuParams);
    }

    void Renderable::SetMaterial(const String& name, const SPtr<Material>& material)
    {
        if (!_mesh)
            return;

        UINT32 numSubMeshes = _mesh->GetProperties().GetNumSubMeshes();
        UINT32 assignedSubMeshed = 0;
        
        for (UINT32 i = 0; i < numSubMeshes; i++)
        {
            const SubMesh& subMesh = _mesh->GetProperties().GetSubMesh();
            if (subMesh.MaterialName == name)
            {
                _materials[i] = material;
                assignedSubMeshed++;
            }
        }

        _markCoreDirty(ActorDirtyFlag::GpuParams);

        if (assignedSubMeshed == 0)
            TE_DEBUG("No submesh currently use the material {" + name + "} in {" + _mesh->GetName() + "}", __FILE__, __LINE__);
    }

    SPtr<Material> Renderable::GetMaterial(UINT32 idx) const
    {
        if (idx >= (UINT32)_materials.size())
            return nullptr;

        return _materials[idx];
    }

    void Renderable::SetLayer(UINT64 layer)
    {
        const bool isPow2 = layer && !((layer - 1) & layer);

        if (!isPow2)
        {
            TE_DEBUG("Invalid layer provided. Only one layer bit may be set. Ignoring.", __FILE__, __LINE__);
            return;
        }

        _layer = layer;
        _markCoreDirty();
    }

    void Renderable::SetCullDistanceFactor(float factor)
    {
        _cullDistanceFactor = factor;
        _markCoreDirty();
    }

    Bounds Renderable::GetBounds() const
    {
        SPtr<Mesh> mesh = GetMesh();

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
