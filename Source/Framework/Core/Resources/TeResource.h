#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
	/**	Base class for all resources. */
	class TE_CORE_EXPORT Resource
	{
	public:
        Resource() {};
		virtual ~Resource() = default;

		/**	Returns the name of the resource. */
		const String& getName() const;

		/**	Sets the name of the resource.  */
		void setName(const String& name);

    protected:
        String _name;
    };
}