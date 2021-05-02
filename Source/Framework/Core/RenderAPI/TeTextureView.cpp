#include "RenderAPI/TeTextureView.h"

namespace te
{
    size_t TextureView::HashFunction::operator()(const TEXTURE_VIEW_DESC &key) const
    {
        size_t seed = 0;
        te_hash_combine(seed, key.MostDetailMip);
        te_hash_combine(seed, key.NumMips);
        te_hash_combine(seed, key.FirstArraySlice);
        te_hash_combine(seed, key.NumArraySlices);
        te_hash_combine(seed, key.Usage);

        return seed;
    }

    bool TextureView::EqualFunction::operator()
        (const TEXTURE_VIEW_DESC &a, const TEXTURE_VIEW_DESC &b) const
    {
        return a.MostDetailMip == b.MostDetailMip && a.NumMips == b.NumMips
            && a.FirstArraySlice == b.FirstArraySlice && a.NumArraySlices == b.NumArraySlices && a.Usage == b.Usage;
    }

    TextureView::TextureView(const TEXTURE_VIEW_DESC& desc)
        : _desc(desc)
    { }
}
