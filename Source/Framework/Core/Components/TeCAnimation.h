#pragma once

#include "TeCorePrerequisites.h"
#include "Animation/TeAnimation.h"
#include "Scene/TeComponent.h"

namespace te
{
    /**
     * @copydoc	Animation
     *
     * @note	Wraps Animation as a Component.
     */
    class TE_CORE_EXPORT CAnimation : public Component
    {
        /** Information about scene objects bound to a specific animation curve. */
        struct SceneObjectMappingInfo
        {
            HBone Bone;
            HSceneObject So;
            bool IsMappedToBone;
        };

    public:
        virtual ~CAnimation();

        /** Return Component type */
        static UINT32 GetComponentType() { return TypeID_Core::TID_CAnimation; }

        /** @copydoc Component::Initialize */
        void Initialize() override;

        /** @copydoc Component::Clone */
        bool Clone(const HComponent& c, const String& suffix = "") override;

        /** @copydoc Component::Clone */
        bool Clone(const HAnimation& c, const String& suffix = "");

        /* @copydoc Component::MarkDirty */
        virtual void MarkDirty() { _internal->MarkCoreDirty(); }

        /**
         * Determines the default clip to play as soon as the component is enabled. If more control over playing clips is needed
         * use the play(), blend() and crossFade() methods to queue clips for playback manually, and setState() method for
         * modify their states individually.
         */
        void SetDefaultClip(const HAnimationClip& clip);

        /** @copydoc SetDefaultClip */
        HAnimationClip GetDefaultClip() const { return _defaultClip; }

        /** @copydoc Animation::SetWrapMode */
        void SetWrapMode(AnimWrapMode wrapMode);

        /** @copydoc SetWrapMode */
        AnimWrapMode GetWrapMode() const { return _wrapMode; }

        /** @copydoc Animation::SetSpeed */
        void SetSpeed(float speed);

        /** @copydoc SetSpeed */
        float GetSpeed() const { return _speed; }

        /** @copydoc Animation::SetBounds */
        void SetBounds(const AABox& bounds) { _internal->SetBounds(bounds); }

        /** @copydoc Animation::GetBounds */
        const AABox& GetBounds() const { return _internal->GetBounds(); }

        /** @copydoc Animation::Play */
        void Play(const HAnimationClip& clip);

        /** @copydoc Animation::BlendAdditive */
        void BlendAdditive(const HAnimationClip& clip, float weight, float fadeLength = 0.0f, UINT32 layer = 0);

        /** @copydoc Animation::Blend1D */
        void Blend1D(const Blend1DInfo& info, float t);

        /** @copydoc Animation::Blend2D */
        void Blend2D(const Blend2DInfo& info, const Vector2& t);

        /** @copydoc Animation::CrossFade */
        void CrossFade(const HAnimationClip& clip, float fadeLength);

        /** @copydoc Animation::Sample */
        void Sample(const HAnimationClip& clip, float time);

        /** @copydoc Animation::Stop */
        void Stop(UINT32 layer);

        /** @copydoc Animation::StopAll */
        void StopAll();

        /** @copydoc Animation::IsPlaying */
        bool IsPlaying() const;

        /** @copydoc Animation::GetState */
        bool GetState(const HAnimationClip& clip, AnimationClipState& state);

        /** @copydoc Animation::SetState */
        void SetState(const HAnimationClip& clip, AnimationClipState state);

        /** Enables or disables culling of the animation when out of view. Culled animation will not be evaluated. */
        void SetEnableCull(bool enable);

        /** Checks whether the animation will be evaluated when it is out of view. */
        bool GetEnableCull() const { return _enableCull; }

        /** Get current preview mode */
        //bool GetPreviewMode() const { return _previewMode; }

        /** @copydoc Animation::GetNumClips */
        UINT32 GetNumClips() const;

        /** @copydoc Animation::GetClip */
        HAnimationClip GetClip(UINT32 idx) const;

        /** Triggered whenever an animation event is reached. */
        Event<void(const HAnimationClip&, const String&)> OnEventTriggered;

        /** @copydoc Animation::ForceDirtyState */
        void ForceDirtyState(AnimDirtyStateFlag state) { _internal->ForceDirtyState(state); }

        /** @copydoc Component::Update */
        void Update();

        /** Returns the Animation implementation wrapped by this component. */
        SPtr<Animation> GetInternal() const { return _internal; }

    protected:
        friend class CBone;
        friend class CRenderable;
        friend class SceneObject;
        using Component::DestroyInternal;

        CAnimation();
        CAnimation(const HSceneObject& parent);

        /**
         * Registers a new bone component, creating a new transform mapping from the bone name to the scene object the
         * component is attached to.
         */
        void AddBone(HBone bone);

        /** Unregisters a bone component, removing the bone -> scene object mapping. */
        void RemoveBone(const HBone& bone);

        /** Called whenever the bone name the Bone component points to changes. */
        void NotifyBoneChanged(const HBone& bone);

        /**
         * Registers a Renderable component with the animation, should be called whenever a Renderable component is added
         * to the same scene object as this component.
         */
        void RegisterRenderable(const HRenderable& renderable);

        /**
         * Removes renderable from the animation component. Should be called when a Renderable component is removed from
         * this scene object.
         */
        void UnregisterRenderable();

        /**
         * Rebuilds internal curve -> property mapping about the currently playing animation clip. This mapping allows the
         * animation component to know which property to assign which values from an animation curve. This should be called
         * whenever playback for a new clip starts, or when clip curves change.
         */
        void RefreshClipMappings();

        /** @copydoc Animation::GetGenericCurveValue */
        bool GetGenericCurveValue(UINT32 curveIdx, float& value);

        /** Re-applies the bounds to the internal animation object, and the relevant renderable object if one exists. */
        void UpdateBounds();

        /** 
        * Creates the internal representation of the Animation and restores the values saved by the Component. 
        * Node : clips must be added manually because this method can't do it properly
        */
        void RestoreInternal(bool previewMode);

        /** Destroys the internal Animation representation. */
        void DestroyInternal();

        /** Callback triggered whenever an animation event is triggered. */
        void EventTriggered(const HAnimationClip& clip, const String& name);

        /**
         * Finds any scene objects that are mapped to bone transforms. Such object's transforms will be affected by
         * skeleton bone animation.
         */
        void SetBoneMappings();

        /**
         * Finds any curves that affect a transform of a specific scene object, and ensures that animation properly updates
         * those transforms. This does not include curves referencing bones.
         */
        void UpdateSceneObjectMapping();

        /** @copydoc Animation::PapCurveToSceneObject */
        void MapCurveToSceneObject(const String& curve, const HSceneObject& so);

        /** @copydoc Animation::UnmapSceneObject */
        void UnmapSceneObject(const HSceneObject& so);

        /** Searches child scene objects for Bone components and returns any found ones. */
        Vector<HBone> FindChildBones();

    protected:
        /** @copydoc Component::OnEnabled */
        void OnEnabled() override;

        /** @copydoc Component::OnEnabled */
        void OnDisabled() override;

        /** @copydoc Component::OnTransformChanged */
        void OnTransformChanged(TransformChangedFlags flags) override;

        /** @copydoc Component::OnDestroyed */
        void OnDestroyed() override;

    protected:
        SPtr<Animation> _internal = nullptr;

        HRenderable _animatedRenderable;

        HAnimationClip _defaultClip;
        HAnimationClip _primaryPlayingClip;
        AnimWrapMode _wrapMode = AnimWrapMode::Loop;
        float _speed = 1.0f;
        bool _enableCull = true;
        bool _previewMode = false; // TODO in my impl, this will always be false

        Vector<SceneObjectMappingInfo> _mappingInfos;
    };
}
