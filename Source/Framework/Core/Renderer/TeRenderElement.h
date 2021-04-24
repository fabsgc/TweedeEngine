#pragma once

#include "TeCorePrerequisites.h"
#include "RenderAPI/TeSubMesh.h"
#include "Mesh/TeMesh.h"
#include "Material/TeMaterial.h"
#include "Renderer/TeRenderable.h"

namespace te
{
    /** Contains all information needed for rendering a single sub-mesh. Closely tied with Renderer. */
    class TE_CORE_EXPORT RenderElement
    {
    public:
        /**	Reference to the mesh to render. */
        SPtr<Mesh> MeshElem;

        /**	Portion of the mesh to render. */
        SubMesh* SubMeshElem = nullptr;

        /**	Material to render the mesh with. */
        SPtr<Material> MaterialElem;

        /** Material data buffer assigned to this material */
        SPtr<GpuParamBlockBuffer> PerMaterialParamBuffer;

        const RenderableProperties* Properties = nullptr;

        /** Index of the technique in the material to render the element with. */
        UINT32 DefaultTechniqueIdx = 0;

        /** Renderer specific value that identifies the type of this renderable element. */
        UINT32 Type = 0;

        /** We can know if the element is instanced or not */
        int InstanceCount = 0;

        /*  All params used by this element for all passes */
        Vector<SPtr<GpuParams>> GpuParamsElem;

        /** Executes the draw call for the render element. */
        virtual void Draw() const = 0;

    protected:
        RenderElement();
        virtual ~RenderElement();
    };
}
