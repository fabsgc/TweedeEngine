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
    };

    /** Provides easier access to AnimationManager. */
    TE_CORE_EXPORT AnimationManager& gAnimation();
}
