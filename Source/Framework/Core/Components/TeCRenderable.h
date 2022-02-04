#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeRenderable.h"
#include "Scene/TeComponent.h"
#include "Mesh/TeMesh.h"

namespace te
{
    /**
     * @copydoc	Renderable
     *
     * @note	Wraps Renderable as a Component.
     */
    class TE_CORE_EXPORT CRenderable : public Component
    {
    public:
        virtual ~CRenderable();

        /** @copydoc Component::GetComponentType */
        static UINT32 GetComponentType() { return TypeID_Core::TID_CRenderable; }

        /** @copydoc Component::Clone */
        bool Clone(const HComponent& c, const String& suffix = "") override;

        /** @copydoc Component::Clone */
        bool Clone(const HRenderable& c, const String& suffix = "");

        /** @copydoc Renderable::SetMesh */
        void SetMesh(HMesh mesh) { _internal->SetMesh(mesh.GetInternalPtr()); }

        /** @copydoc Renderable::SetMesh */
        void SetMesh(SPtr<Mesh> mesh) { _internal->SetMesh(mesh); }

        /** @copydoc Renderable::GetMesh */
        SPtr<Mesh> GetMesh() const { return _internal->GetMesh(); }

        /** @copydoc Renderable::SetMaterial */
        void SetMaterial(UINT32 idx, HMaterial material) { _internal->SetMaterial(idx, material.GetInternalPtr()); }

        /** @copydoc Renderable::SetMaterial */
        void SetMaterial(HMaterial material, bool all = false) { _internal->SetMaterial(material.GetInternalPtr(), all); }

        /** @copydoc Renderable::SetMaterial */
        void SetMaterial(const String& name, HMaterial material) { _internal->SetMaterial(name, material.GetInternalPtr()); }

        /** @copydoc Renderable::SetMaterial */
        void SetMaterial(UINT32 idx, SPtr<Material> material) { _internal->SetMaterial(idx, material); }

        /** @copydoc Renderable::SetMaterial */
        void SetMaterial(SPtr<Material> material, bool all = false) { _internal->SetMaterial(material, all); }

        /** @copydoc Renderable::SetMaterial */
        void SetMaterial(const String& name, SPtr<Material> material) { _internal->SetMaterial(name, material); }

        /** @copydoc Renderable::RemoveMaterial */
        void RemoveMaterial(const SPtr<Material>& material) { _internal->RemoveMaterial(material); }

        /** @copydoc Renderable::GetMaterial */
        SPtr<Material> GetMaterial(UINT32 idx) const { return _internal->GetMaterial(idx); }

        /** @copydoc Renderable::SetMaterials */
        void SetMaterials(const Vector<SPtr<Material>>& materials) { _internal->SetMaterials(materials); }

        /** @copydoc Renderable::SetMaterials */
        void SetMaterials(const Vector<HMaterial>& materials);

        /** @copydoc Renderable::IsUsingMaterial */
        bool IsUsingMaterial(const SPtr<Material>& material) { return _internal->IsUsingMaterial(material); }

        /** @copydoc Renderable::UpdateMaterials */
        void UpdateMaterials() { _internal->UpdateMaterials(); }

        /** @copydoc Renderable::ClearAllMaterials */
        void ClearAllMaterials() { _internal->ClearAllMaterials(); }

        /** @copydoc Renderable::GetMaterials */
        const Vector<SPtr<Material>>& GetMaterials() { return _internal->GetMaterials(); }

        /** @copydoc Renderable::SetCullDistanceFactor */
        void SetCullDistanceFactor(float factor) { _internal->SetCullDistanceFactor(factor); }

        /** @copydoc Renderable::GetCullDistanceFactor */
        float GetCullDistanceFactor() const { return _internal->GetCullDistanceFactor(); }

        /** @copydoc Renderable::SetWriteVelocity */
        void SetWriteVelocity(bool enable) { _internal->SetWriteVelocity(enable); }

        /** @copydoc Renderable::GetWriteVelocity */
        bool GetWriteVelocity() const { return _internal->GetWriteVelocity(); }

        /** @copydoc Renderable::SetUseForDynamicEnvMapping */
        void SetUseForDynamicEnvMapping(bool use) { _internal->SetUseForDynamicEnvMapping(use); }

        /** @copydoc Renderable::GetUseForDynamicEnvMapping */
        float GetUseForDynamicEnvMapping() const { return _internal->GetUseForDynamicEnvMapping(); }

        /** @copydoc Renderable::SetLayer */
        void SetLayer(UINT64 layer) { _internal->SetLayer(layer); }

        /** @copydoc Renderable::GetLayer */
        UINT64 GetLayer() const { return _internal->GetLayer(); }

        /** @copydoc SetInstancing */
        void SetInstancing(bool instancing) { _internal->SetInstancing(instancing); }

        /** @copydoc SetInstancing */
        bool GetInstancing() const { return _internal->GetInstancing(); }

        /**	Gets world bounds of the mesh rendered by this object. */
        Bounds GetBounds() const;

        /** @copydoc Component::CalculateBounds */
        bool CalculateBounds(Bounds& bounds) override;

        /**	@copydoc Renderable::GetMatrix */
        Matrix4 GetMatrix() const { return _internal->GetMatrix(); }

        /**	@copydoc Renderable::GetMatrixNoScale */
        const Matrix4& GetMatrixNoScale() const { return _internal->GetMatrixNoScale(); }

        /** Returns the internal renderable that is used for majority of operations by this component. */
        SPtr<Renderable> GetInternal() const { return _internal; }

        /** @copydoc SceneActor::SetActive */
        void SetActive(bool active) { _internal->SetActive(active); }

        /** @copydoc SceneActor::GetActive */
        bool GetActive() const { return _internal->GetActive(); }

        /** @copydoc Renderable::IsAnimated */
        bool IsAnimated() const { return _internal->IsAnimated(); }

        /* @copydoc Component::MarkDirty */
        virtual void MarkDirty() 
        { 
            _internal->SetTransform(_internal->GetTransform());
            _internal->_markCoreDirty(); 
        }

    protected:
        /** Registers an Animation component that will be used for animating the renderable's mesh. */
        void RegisterAnimation(const HAnimation& animation);

        /** Removes the Animation component, making the renderable rendered as a static object. */
        void UnregisterAnimation();

    protected:
        friend class SceneObject;
        friend class CAnimation;

        CRenderable();
        CRenderable(const HSceneObject& parent);

        /** @copydoc Component::Instantiate */
        void Instantiate() override;

        /** @copydoc Component::OnInitialized */
        void OnInitialized() override;

        /** @copydoc Component::OnEnabled */
        void OnEnabled() override;

        /** @copydoc Component::OnEnabled */
        void OnDisabled() override;

        /** @copydoc Component::OnTransformChanged */
        void OnTransformChanged(TransformChangedFlags flags) override;

        /** @copydoc Component::OnDestroyed */
        void OnDestroyed() override;

    protected:
        mutable SPtr<Renderable> _internal;
        HAnimation _animation;
    };
}
