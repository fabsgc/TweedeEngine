#include "TeShadowRendering.h"

namespace te
{
    ShadowRendering::ShadowRendering(UINT32 shadowMapSize)
        : _shadowMapSize(shadowMapSize)
    { }

    void ShadowRendering::SetShadowMapSize(UINT32 size)
    {
        if (_shadowMapSize == size)
            return;

        _shadowMapSize = size;
    }
}
