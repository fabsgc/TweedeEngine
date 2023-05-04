#include "TeTextureAtlasLayout.h"
#include "Utility/TeBitwise.h"

namespace te
{
    bool TextureAtlasLayout::AddElement(UINT32 width, UINT32 height, UINT32& x, UINT32& y)
    {
        if (width == 0 || height == 0)
        {
            x = 0;
            y = 0;
            return true;
        }

        // Try adding without expanding, if that fails try to expand
        if (!AddToNode(0, width, height, x, y, false))
        {
            if (!AddToNode(0, width, height, x, y, true))
                return false;
        }

        // Update size to cover all nodes
        if (_pow2)
        {
            _width = std::max(_width, Bitwise::NextPow2(x + width));
            _height = std::max(_height, Bitwise::NextPow2(y + height));
        }
        else
        {
            _width = std::max(_width, x + width);
            _height = std::max(_height, y + height);
        }

        return true;
    }

    void TextureAtlasLayout::Clear()
    {
        _nodes.clear();
        _nodes.push_back(TexAtlasNode(0, 0, _width, _height));

        _width = _initialWidth;
        _height = _initialHeight;
    }

    bool TextureAtlasLayout::AddToNode(UINT32 nodeIdx, UINT32 width, UINT32 height, UINT32& x, UINT32& y, bool allowGrowth)
    {
        TexAtlasNode* node = &_nodes[nodeIdx];
        float aspect = node->width / (float)node->height;

        if (node->children[0] != (UINT32)-1)
        {
            if (AddToNode(node->children[0], width, height, x, y, allowGrowth))
                return true;

            return AddToNode(node->children[1], width, height, x, y, allowGrowth);
        }
        else
        {
            if (node->nodeFull)
                return false;

            if (width > node->width || height > node->height)
                return false;

            if (!allowGrowth)
            {
                if (node->x + width > _width || node->y + height > _height)
                    return false;
            }

            if (width == node->width && height == node->height)
            {
                x = node->x;
                y = node->y;
                node->nodeFull = true;

                return true;
            }

            float dw = (float)(node->width - width);
            float dh = (node->height - height) * aspect;

            UINT32 nextChildIdx = (UINT32)_nodes.size();
            node->children[0] = nextChildIdx;
            node->children[1] = nextChildIdx + 1;

            TexAtlasNode nodeCopy = *node;
            node = nullptr; // Undefined past this point
            if (dw > dh)
            {
                _nodes.emplace_back(nodeCopy.x, nodeCopy.y, width, nodeCopy.height);
                _nodes.emplace_back(nodeCopy.x + width, nodeCopy.y, nodeCopy.width - width, nodeCopy.height);
            }
            else
            {
                _nodes.emplace_back(nodeCopy.x, nodeCopy.y, nodeCopy.width, height);
                _nodes.emplace_back(nodeCopy.x, nodeCopy.y + height, nodeCopy.width, nodeCopy.height - height);
            }

            return AddToNode(nodeCopy.children[0], width, height, x, y, allowGrowth);
        }
    }

    Vector<TextureAtlasUtility::Page> TextureAtlasUtility::CreateAtlasLayout(Vector<Element>& elements, UINT32 width,
        UINT32 height, UINT32 maxWidth, UINT32 maxHeight, bool pow2)
    {
        for (size_t i = 0; i < elements.size(); i++)
        {
            elements[i].output.idx = (UINT32)i; // Preserve original index before sorting
            elements[i].output.page = -1;
        }

        std::sort(elements.begin(), elements.end(),
        [](const Element& a, const Element& b) {
            return a.input.width * a.input.height > b.input.width * b.input.height;
        });

        Vector<TextureAtlasLayout> layouts;
        UINT32 remainingCount = (UINT32)elements.size();
        while (remainingCount > 0)
        {
            layouts.push_back(TextureAtlasLayout(width, height, maxWidth, maxHeight, pow2));
            TextureAtlasLayout& curLayout = layouts.back();

            // Find largest unassigned element that fits
            UINT32 sizeLimit = std::numeric_limits<UINT32>::max();
            while (true)
            {
                UINT32 largestId = (UINT32) - 1;

                // Assumes elements are sorted from largest to smallest
                for (UINT32 i = 0; i < (UINT32)elements.size(); i++)
                {
                    if (elements[i].output.page == -1)
                    {
                        UINT32 size = elements[i].input.width * elements[i].input.height;
                        if (size < sizeLimit)
                        {
                            largestId = i;
                            break;
                        }
                    }
                }

                if (largestId == (UINT32)-1)
                    break; // Nothing fits, start a new page

                Element& element = elements[largestId];

                // Check if an element is too large to ever fit
                if (element.input.width > maxWidth || element.input.height > maxHeight)
                {
                    TE_DEBUG("Some of the provided elements don't fit in an atlas of provided size. "
                        "Returning empty array of pages.");
                    return Vector<Page>();
                }

                if (curLayout.AddElement(element.input.width, element.input.height, element.output.x, element.output.y))
                {
                    element.output.page = (UINT32)layouts.size() - 1;
                    remainingCount--;
                }
                else
                {
                    sizeLimit = element.input.width * element.input.height;
                }
            }
        }

        Vector<Page> pages;
        for (auto& layout : layouts)
            pages.push_back({ layout.GetWidth(), layout.GetHeight() });

        return pages;
    }
}
