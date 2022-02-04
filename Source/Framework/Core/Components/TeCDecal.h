#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeDecal.h"
#include "Scene/TeComponent.h"
#include "Material/TeMaterial.h"

namespace te
{
    /**
     * @copydoc	Decal
     *
     * @note	Wraps Decal as a Component.
     */
    class TE_CORE_EXPORT CDecal : public Component
    {
    public:
        virtual ~CDecal();

        /** @copydoc Component::GetComponentType */
        static UINT32 GetComponentType() { return TypeID_Core::TID_CDecal; }

        /** @copydoc Component::Clone */
        bool Clone(const HComponent& c, const String& suffix = "") override;

        /** @copydoc Component::Clone */
        bool Clone(const HDecal& c, const String& suffix = "");

        /** Returns the internal decal that is used for majority of operations by this component. */
        SPtr<Decal> GetInternal() const { return _internal; }

        /** @copydoc Decal::SetMaterial */
        void SetMaterial(const HMaterial& material) { _internal->SetMaterial(material.GetInternalPtr()); }

        /** @copydoc Decal::SetMaterial */
        void SetMaterial(const SPtr<Material>& material) { _internal->SetMaterial(material); }

        /** @copydoc SetMaterial */
        const SPtr<Material>& GetMaterial() const { return _internal->GetMaterial(); }

        /** @copydoc Decal::SetSize */
        void SetSize(const Vector2& size) { _internal->SetSize(size); }

        /** @copydoc SetSize */
        Vector2 GetSize() const { return _internal->GetSize(); }

        /** @copydoc Decal::SetMaxDistance */
        void SetMaxDistance(float distance) { _internal->SetMaxDistance(distance); }

        /** @copydoc SetMaxDistance */
        float GetMaxDistance() const { return _internal->GetMaxDistance(); }

        /** @copydoc Decal::SetLayer */
        void SetLayer(UINT64 layer) { _internal->SetLayer(layer); }

        /** @copydoc SetLayer() */
        UINT64 GetLayer() const { return _internal->GetLayer(); }

        /** @copydoc Decal::SetLayerMask */
        void SetLayerMask(UINT32 mask) { _internal->SetLayerMask(mask); }

        /** @copydoc SetLayerMask */
        UINT32 GetLayerMask() const { return _internal->GetLayerMask(); }

        /** @copydoc SceneActor::SetActive */
        void SetActive(bool active) { _internal->SetActive(active); }

        /** @copydoc SceneActor::GetActive */
        bool GetActive() const { return _internal->GetActive(); }

        /* @copydoc Component::MarkDirty */
        virtual void MarkDirty() 
        { 
            _internal->SetTransform(_internal->GetTransform());
            _internal->_markCoreDirty(); 
        }

    protected:
        friend class SceneObject;

        CDecal();
        CDecal(const HSceneObject& parent);

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
        mutable SPtr<Decal> _internal;
    };
}
