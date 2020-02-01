#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeRenderable.h"
#include "Scene/TeComponent.h"

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
        virtual ~CRenderable() = default;

        /** @copydoc Component::Initialize */
        void Initialize() override;

        static UINT32 GetComponentType() { return TID_CRenderable; }

        /** @copydoc Renderable::SetMesh */
        void SetMesh(HMesh mesh);

        /** @copydoc Renderable::SetMesh */
        void SetMesh(SPtr<Mesh> mesh);

        /** @copydoc Renderable::GetMesh */
        SPtr<Mesh> GetMesh() const { return _internal->GetMesh(); }

        /** @copydoc Renderable::SetCullDistanceFactor */
        void SetCullDistanceFactor(float factor) { _internal->SetCullDistanceFactor(factor); }

        /** @copydoc Renderable::GetCullDistanceFactor */
        float GetCullDistanceFactor() const { return _internal->GetCullDistanceFactor(); }

        /**	Gets world bounds of the mesh rendered by this object. */
        Bounds GetBounds() const;

        /** @copydoc Component::CalculateBounds */
        bool CalculateBounds(Bounds& bounds) override;

         /** Returns the internal renderable that is used for majority of operations by this component. */
        SPtr<Renderable> _getInternal() const { return _internal; }

    protected:
        mutable SPtr<Renderable> _internal;

    protected:
        friend class SceneObject;

        CRenderable(const HSceneObject& parent);

        /** @copydoc Component::_instantiate */
        void _instantiate() override;

        /** @copydoc Component::OnInitialized */
        void OnInitialized() override;

        /** @copydoc Component::OnDestroyed */
        void OnDestroyed() override;

    public:
        /** @copydoc Component::Update */
        void Update() override { }

    protected:
        CRenderable();
    };
}
