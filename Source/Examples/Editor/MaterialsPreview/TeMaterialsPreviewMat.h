#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeRendererMaterial.h"

namespace te
{
    /** Shader that performs material's preview for opaque objects. */
    class PreviewOpaqueMat : public RendererMaterial<PreviewOpaqueMat>
    {
        RMAT_DEF(BuiltinShader::PreviewOpaque);

    public:
        PreviewOpaqueMat();
    };

    /** Shader that performs material's preview for transparent objects. */
    class PreviewTransparentMat : public RendererMaterial<PreviewTransparentMat>
    {
        RMAT_DEF(BuiltinShader::PreviewTransparent);

    public:
        PreviewTransparentMat();
    };
}
