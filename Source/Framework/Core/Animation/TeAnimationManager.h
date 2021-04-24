#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"
#include "Math/TeConvexVolume.h"

namespace te
{
    struct AnimationProxy;

    /** Contains skeleton poses for all animations evaluated on a single frame. */
    struct EvaluatedAnimationData
    {
        /** Contains meta-data about a calculated skeleton pose. Actual data maps to the @p transforms buffer. */
        struct PoseInfo
        {
            UINT64 AnimId;
            UINT32 StartIdx;
            UINT32 NumBones;
        };

        /** Contains meta-data about where calculated animation data is stored. */
        struct AnimInfo
        {
            PoseInfo PoseInfos;
        };

        /**
         * Maps animation ID to a animation information structure, which points to relevant skeletal or morph shape data.
         */
        UnorderedMap<UINT64, AnimInfo> Infos;

        /** Global joint transforms for all skeletons in the scene. */
        Vector<Matrix4> Transforms;
    };

    /**
     * Keeps track of all active animations, queues animation thread tasks and synchronizes data between simulation, core
     * and animation threads.
     */
    class TE_CORE_EXPORT AnimationManager : public Module<AnimationManager>
    {
    public:
        TE_MODULE_STATIC_HEADER_MEMBER(AnimationManager)

        /** Constructor */
        AnimationManager();

        /** Pauses or resumes the animation evaluation. */
        void SetPaused(bool paused);

        /** Pauses or resumes the animation evaluation. */
        void TogglePaused();

        /** Ask to the manager to update _animData */
        void SetAnimDataDirty();

        /**
         * Evaluates animations for all animated objects, and returns the evaluated skeleton bone poses and morph shape
         * meshes that can be passed along to the renderer.
         *
         * @return	Evaluated animation data for this frame. 
         */
        const EvaluatedAnimationData* Update();

        /**
         * Determines how often to evaluate animations. If rendering is not running at adequate framerate the animation
         * could end up being evaluated less times than specified here.
         *
         * @param[in]	fps		Number of frames per second to evaluate the animation. Default is 60.
         */
        void SetUpdateRate(UINT32 fps);

    private:
        friend class Animation;

        /**
         * Registers a new animation and returns a unique ID for it. Must be called whenever an Animation is constructed.
         */
        UINT64 RegisterAnimation(Animation* anim);

        /** Unregisters an animation with the specified ID. Must be called before an Animation is destroyed. */
        void UnregisterAnimation(UINT64 id);

        /**
         * Evaluates animation for a single object and writes the result in the currently active write buffer.
         *
         * @param[in]	anim		Proxy representing the animation to evaluate.
         * @param[in]	boneIdx		Index in the output buffer in which to write evaluated bone information. This will be
         *							automatically advanced by the number of written bone transforms.
         */
        void EvaluateAnimation(AnimationProxy* anim, UINT32& boneIdx);

    private:
        UINT64 _nextId = 1;
        UnorderedMap<UINT64, Animation*> _animations;

        float _updateRate = 1.0f / 60.0f;
        float _animationTime = 0.0f;
        float _lastAnimationUpdateTime = 0.0f;
        float _nextAnimationUpdateTime = 0.0f;
        float _lastAnimationDeltaTime = 0.0f;
        bool  _paused = true;

        Vector<SPtr<AnimationProxy>> _proxies;
        Vector<ConvexVolume> _cullFrustums;

        // If we change a mesh (so a skeleton, animData info might be deprecated, we must update them)
        bool _animDataDirty = true;
        EvaluatedAnimationData _animData;
    };

    /** Provides easier access to AnimationManager. */
    TE_CORE_EXPORT AnimationManager& gAnimationManager();
}
