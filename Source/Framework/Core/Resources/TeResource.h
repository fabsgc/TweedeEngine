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

        /**	Returns the UUID of the resource. */
		const te::UUID& getUUID() const;

		/**	Sets the UUID of the resource.  */
		void setUUID(const te::UUID& uuid);

    protected:
        String _name;
        te::UUID _uuid;
    };
}