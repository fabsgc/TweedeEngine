#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"
#include "Resources/TeBuiltinResources.h"
#include <any>

namespace te
{
    class RendererMaterialBase;
    struct RendererMaterialMetaData;

    /**	Information used for initializing a renderer material managed by this module. */
    struct RendererMaterialData
    {
        RendererMaterialMetaData* MetaData;
        std::any ShaderPath;
    };

    /**	Initializes and handles all renderer materials. */
    class TE_CORE_EXPORT RendererMaterialManager : public Module<RendererMaterialManager>
    {
    public:
        RendererMaterialManager();
        virtual ~RendererMaterialManager();

        TE_MODULE_STATIC_HEADER_MEMBER(RendererMaterialManager)

        /**	Registers a new material that should be initialized on module start-up. */
        static void RegisterMaterial(RendererMaterialMetaData* metaData, const std::any& shaderPath);

        /**	Returns a list in which are all materials managed by this module. */
        static Vector<RendererMaterialData>& GetMaterials();
    private:
        template<class T>
        friend class RendererMaterial;
        friend class RendererMaterialBase;

        /**	Initializes all materials on the core thread. */
        static void InitMaterials(const Vector<SPtr<Shader>>& shaders);

        /**	Destroys all materials */
        static void DestroyMaterials();
    };
}
