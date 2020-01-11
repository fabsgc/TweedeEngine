#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    /** Core objects provides a standardized way to initialize/destroy objects, and a way to specify dependant CoreObjects. */
	class TE_CORE_EXPORT CoreObject
	{
    public:
        /** Frees all the data held by this object. */
        virtual void Destroy() {}

		/**
		 * Initializes all the internal resources of this object. Must be called right after construction. Generally you
		 * should call this from a factory method to avoid the issue where user forgets to call it.
		 */
		virtual void Initialize() {}

        /** Returns a shared_ptr version of "this" pointer. */
		SPtr<CoreObject> GetThisPtr() const { return _this.lock(); };

        /**
		 * Sets a shared this pointer to this object. This must be called immediately after construction, but before
		 * initialize().
		 *
		 * @note	This should be called by the factory creation methods so user doesn't have to call it manually.
		 */
		void SetThisPtr(SPtr<CoreObject> ptrThis);

    private:
        std::weak_ptr<CoreObject> _this;
    };
}
