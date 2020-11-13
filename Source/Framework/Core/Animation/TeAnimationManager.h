#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"

namespace te
{
    /**
     * Keeps track of all active animations, queues animation thread tasks and synchronizes data between simulation, core
     * and animation threads.
     */
    class TE_CORE_EXPORT AnimationManager : public Module<AnimationManager>
    {
    public:
        AnimationManager();

        /** Pauses or resumes the animation evaluation. */
        void SetPaused(bool paused);

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

    private:
        UINT64 _nextId = 1;
        UnorderedMap<UINT64, Animation*> _animations;

        float _updateRate = 1.0f / 60.0f;
        float _animationTime = 0.0f;
        float _lastAnimationUpdateTime = 0.0f;
        float _nextAnimationUpdateTime = 0.0f;
        float _lastAnimationDeltaTime = 0.0f;
        bool  _paused = false;
    };

    /** Provides easier access to AnimationManager. */
    TE_CORE_EXPORT AnimationManager& gAnimationManager();
}
