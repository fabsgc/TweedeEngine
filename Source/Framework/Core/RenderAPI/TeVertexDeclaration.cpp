#include "TeVertexDeclaration.h"
#include "Image/TeColor.h"
#include "RenderAPI/TeHardwareBufferManager.h"

namespace te
{
    UINT32 VertexDeclaration::NextFreeId = 0;

    VertexElement::VertexElement(UINT32 source, UINT32 offset,
        VertexElementType theType, VertexElementSemantic semantic, UINT32 index, UINT32 instanceStepRate)
        : _source(source)
        , _offset(offset)
        , _type(theType)
        , _semantic(semantic)
        , _index(index)
        , _instanceStepRate(instanceStepRate)
    {
    }

    UINT32 VertexElement::GetSize(void) const
    {
        return GetTypeSize(_type);
    }

    UINT32 VertexElement::GetTypeSize(VertexElementType etype)
    {
        switch (etype)
        {
        case VET_COLOR:
        case VET_COLOR_ABGR:
        case VET_COLOR_ARGB:
            return sizeof(float) * 4;
        case VET_UBYTE4_NORM:
            return sizeof(UINT32);
        case VET_FLOAT1:
            return sizeof(float);
        case VET_FLOAT2:
            return sizeof(float) * 2;
        case VET_FLOAT3:
            return sizeof(float) * 3;
        case VET_FLOAT4:
            return sizeof(float) * 4;
        case VET_USHORT1:
            return sizeof(UINT16);
        case VET_USHORT2:
            return sizeof(UINT16) * 2;
        case VET_USHORT4:
            return sizeof(UINT16) * 4;
        case VET_SHORT1:
            return sizeof(INT16);
        case VET_SHORT2:
            return sizeof(INT16) * 2;
        case VET_SHORT4:
            return sizeof(INT16) * 4;
        case VET_UINT1:
            return sizeof(UINT32);
        case VET_UINT2:
            return sizeof(UINT32) * 2;
        case VET_UINT3:
            return sizeof(UINT32) * 3;
        case VET_UINT4:
            return sizeof(UINT32) * 4;
        case VET_INT4:
            return sizeof(INT32) * 4;
        case VET_INT1:
            return sizeof(INT32);
        case VET_INT2:
            return sizeof(INT32) * 2;
        case VET_INT3:
            return sizeof(INT32) * 3;
        case VET_UBYTE4:
            return sizeof(UINT8) * 4;
        default:
            break;
        }

        return 0;
    }

    UINT32 VertexElement::GetTypeCount(VertexElementType etype)
    {
        switch (etype)
        {
        case VET_COLOR:
        case VET_COLOR_ABGR:
        case VET_COLOR_ARGB:
            return 4;
        case VET_FLOAT1:
        case VET_SHORT1:
        case VET_USHORT1:
        case VET_INT1:
        case VET_UINT1:
            return 1;
        case VET_FLOAT2:
        case VET_SHORT2:
        case VET_USHORT2:
        case VET_INT2:
        case VET_UINT2:
            return 2;
        case VET_FLOAT3:
        case VET_INT3:
        case VET_UINT3:
            return 3;
        case VET_FLOAT4:
        case VET_SHORT4:
        case VET_USHORT4:
        case VET_INT4:
        case VET_UINT4:
        case VET_UBYTE4:
        case VET_UBYTE4_NORM:
            return 4;
        default:
            break;
        }

        TE_ASSERT_ERROR(false, "Invalid type");
        return 0;
    }

    VertexElementType VertexElement::GetBestColorVertexElementType()
    {
        // We can't know the specific type right now, so pick a type based on platform
#if TE_PLATFORM == TE_PLATFORM_WIN32
        return VET_COLOR_ARGB; // prefer D3D format on Windows
#else
        return VET_COLOR_ABGR; // prefer GL format on everything else
#endif
    }

    bool VertexElement::operator== (const VertexElement& rhs) const
    {
        if (_type != rhs._type || _index != rhs._index || _offset != rhs._offset ||
            _semantic != rhs._semantic || _source != rhs._source || _instanceStepRate != rhs._instanceStepRate)
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    bool VertexElement::operator!= (const VertexElement& rhs) const
    {
        return !(*this == rhs);
    }

    size_t VertexElement::GetHash(const VertexElement& element)
    {
        size_t hash = 0;
        te_hash_combine(hash, element._type);
        te_hash_combine(hash, element._index);
        te_hash_combine(hash, element._offset);
        te_hash_combine(hash, element._semantic);
        te_hash_combine(hash, element._source);
        te_hash_combine(hash, element._instanceStepRate);

        return hash;
    }

    VertexDeclarationProperties::VertexDeclarationProperties(const Vector<VertexElement>& elements)
    {
        for (auto& elem : elements)
        {
            VertexElementType type = elem.GetType();

            if (elem.GetType() == VET_COLOR)
            {
                type = VertexElement::GetBestColorVertexElementType();
            }

            _elementList.push_back(VertexElement(elem.GetStreamIdx(), elem.GetOffset(), type, elem.GetSemantic(),
                elem.GetSemanticIdx(), elem.GetInstanceStepRate()));
        }
    }

    bool VertexDeclarationProperties::operator== (const VertexDeclarationProperties& rhs) const
    {
        if (_elementList.size() != rhs._elementList.size())
        {
            return false;
        }

        auto myIter = _elementList.begin();
        auto theirIter = rhs._elementList.begin();

        for (; myIter != _elementList.end() && theirIter != rhs._elementList.end(); ++myIter, ++theirIter)
        {
            if (!(*myIter == *theirIter))
            {
                return false;
            }
        }

        return true;
    }

    bool VertexDeclarationProperties::operator!= (const VertexDeclarationProperties& rhs) const
    {
        return !(*this == rhs);
    }

    const VertexElement* VertexDeclarationProperties::GetElement(UINT32 index) const
    {
        assert(index < _elementList.size() && "Index out of bounds");

        auto iter = _elementList.begin();
        for (UINT32 i = 0; i < index; ++i)
        {
            ++iter;
        }

        return &(*iter);
    }

    const VertexElement* VertexDeclarationProperties::FindElementBySemantic(VertexElementSemantic sem, UINT32 index) const
    {
        for (auto& elem : _elementList)
        {
            if (elem.GetSemantic() == sem && elem.GetSemanticIdx() == index)
            {
                return &elem;
            }
        }

        return nullptr;
    }

    Vector<VertexElement> VertexDeclarationProperties::FindElementsBySource(UINT32 source) const
    {
        Vector<VertexElement> retList;
        for (auto& elem : _elementList)
        {
            if (elem.GetStreamIdx() == source)
            {
                retList.push_back(elem);
            }
        }

        return retList;
    }

    UINT32 VertexDeclarationProperties::GetVertexSize(UINT32 source) const
    {
        UINT32 size = 0;

        for (auto& elem : _elementList)
        {
            if (elem.GetStreamIdx() == source)
            {
                size += elem.GetSize();
            }
        }

        return size;
    }

    VertexDeclaration::VertexDeclaration(const Vector<VertexElement>& elements, GpuDeviceFlags deviceMask)
        : _properties(elements)
    { }

    void VertexDeclaration::Initialize()
    {
        _id = NextFreeId++;
        CoreObject::Initialize();
    }

    SPtr<VertexDeclaration> VertexDeclaration::Create(const SPtr<VertexDataDesc>& desc, GpuDeviceFlags deviceMask)
    {
        return HardwareBufferManager::Instance().CreateVertexDeclaration(desc, deviceMask);
    }

    bool VertexDeclaration::IsCompatible(const SPtr<VertexDeclaration>& shaderDecl)
    {
        const Vector<VertexElement>& shaderElems = shaderDecl->GetProperties().GetElements();
        const Vector<VertexElement>& bufferElems = GetProperties().GetElements();

        for (auto shaderIter = shaderElems.begin(); shaderIter != shaderElems.end(); ++shaderIter)
        {
            const VertexElement* foundElement = nullptr;
            for (auto bufferIter = bufferElems.begin(); bufferIter != bufferElems.end(); ++bufferIter)
            {
                if (shaderIter->GetSemantic() == bufferIter->GetSemantic() && shaderIter->GetSemanticIdx() == bufferIter->GetSemanticIdx())
                {
                    foundElement = &(*bufferIter);
                    break;
                }
            }

            if (foundElement == nullptr)
            {
                return false;
            }
        }

        return true;
    }

    Vector<VertexElement> VertexDeclaration::GetMissingElements(const SPtr<VertexDeclaration>& shaderDecl)
    {
        Vector<VertexElement> missingElements;

        const Vector<VertexElement>& shaderElems = shaderDecl->GetProperties().GetElements();
        const Vector<VertexElement>& bufferElems = GetProperties().GetElements();

        for (auto shaderIter = shaderElems.begin(); shaderIter != shaderElems.end(); ++shaderIter)
        {
            const VertexElement* foundElement = nullptr;
            for (auto bufferIter = bufferElems.begin(); bufferIter != bufferElems.end(); ++bufferIter)
            {
                if (shaderIter->GetSemantic() == bufferIter->GetSemantic() && shaderIter->GetSemanticIdx() == bufferIter->GetSemanticIdx())
                {
                    foundElement = &(*bufferIter);
                    break;
                }
            }

            if (foundElement == nullptr)
                missingElements.push_back(*shaderIter);
        }

        return missingElements;
    }

    String ToString(const VertexElementSemantic& val)
    {
        switch (val)
        {
        case VES_POSITION:
            return "POSITION";
        case VES_BLEND_WEIGHTS:
            return "BLEND_WEIGHTS";
        case VES_BLEND_INDICES:
            return "BLEND_INDICES";
        case VES_NORMAL:
            return "NORMAL";
        case VES_COLOR:
            return "COLOR";
        case VES_TEXCOORD:
            return "TEXCOORD";
        case VES_BITANGENT:
            return "BITANGENT";
        case VES_TANGENT:
            return "TANGENT";
        case VES_POSITIONT:
            return "POSITIONT";
        case VES_PSIZE:
            return "PSIZE";
        }

        return "";
    }
}
