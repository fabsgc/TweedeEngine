#include "TeMaterialsPreview.h"

namespace te
{
    MaterialsPreview::Preview::Preview()
    { }

    MaterialsPreview::MaterialsPreview()
        : _opaqueMat(nullptr)
        , _transparentMat(nullptr)
    { }

    const RendererUtility::RenderTextureData& MaterialsPreview::GetPreview(WPtr<Material> material)
    {
        auto it = _previews.find(material);
        if (it != _previews.end())
        {
            if (it->second.IsDirty)
                DrawMaterial(material, it->second);

            return *(it->second.MatPreview);
        }
        else
        {
            Preview preview;
            _previews[material] = preview;

            DrawMaterial(material, preview);

            return *(preview.MatPreview);
        }
    }

    void MaterialsPreview::MarkDirty(WPtr<Material> material)
    {
        auto it = _previews.find(material);
        if (it != _previews.end())
            it->second.IsDirty = true;
        else
            _previews[material] = Preview();
    }

    void MaterialsPreview::DeletePreview(WPtr<Material> material)
    { 
        auto it = _previews.find(material);
        if (it != _previews.end())
            _previews.erase(it);
    }

    void MaterialsPreview::DrawMaterial(const WPtr<Material>& material, Preview& preview)
    { 
        preview.IsDirty = false;
    }
}
