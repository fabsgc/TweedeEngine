#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeRendererMaterial.h"
#include "Picking/TePickingUtils.h"

namespace te
{
    /** Shader that performs GPU selection rendering blit. */
    class SelectionBlitMat : public RendererMaterial<SelectionBlitMat>
    {
        RMAT_DEF(BuiltinShader::BlitSelection);

    public:
        SelectionBlitMat();

        /** @copydoc RendererMaterialBase::Initialize */
        void Initialize() override;

        /** Blit blurred outline */
        void Execute(const SPtr<Texture>& viewport, const SPtr<Texture>& outline, const SPtr<Texture>& blurredOutline);
    };
}
