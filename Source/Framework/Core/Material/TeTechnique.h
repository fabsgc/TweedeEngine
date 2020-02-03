#include "TeCorePrerequisites.h"
#include "CoreUtility/TeCoreObject.h"

namespace te
{
    class TE_CORE_EXPORT Technique : public CoreObject
    {
    public:
        Technique() = default;
        ~Technique() = default;

    protected:
        Vector<SPtr<Pass>> _passes;
    };
}
