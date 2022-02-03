#include "TeRendererDecal.h"
#include "TeRendererRenderable.h"
#include "Renderer/TeDecal.h"
#include "Mesh/TeMesh.h"
#include "Renderer/TeRendererUtility.h"

namespace te
{ 
    DecalParamDef gDecalParamDef;
    PerCallParamDef gPerCallParamDef;

    void DecalRenderElement::Draw() const
    {
        gRendererUtility().Draw(MeshElem, *SubMeshElem);
    }

    RendererDecal::RendererDecal()
    {
        DecalParamBuffer = gDecalParamDef.CreateBuffer();
        PerObjectParamBuffer = gPerObjectParamDef.CreateBuffer();
        PerCallParamBuffer = gPerFrameParamDef.CreateBuffer();
    }

    void RendererDecal::UpdatePerObjectBuffer()
    {
        // TODO
    }

    void RendererDecal::UpdatePerCallBuffer(const Matrix4& viewProj) const
    {
        const Matrix4 worldViewProjMatrix = viewProj * DecalPtr->GetMatrix();
        gPerCallParamDef.gMatWorldViewProj.Set(PerCallParamBuffer, worldViewProjMatrix);
    }
}
