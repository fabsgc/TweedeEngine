#include "RenderAPI/TeHardwareBufferManager.h"
#include "RenderAPI/TeVertexDeclaration.h"
#include "RenderAPI/TeVertexDataDesc.h"
#include "RenderAPI/TeGpuParamBlockBuffer.h"
#include "RenderAPI/TeGpuParams.h"

namespace te
{
    HardwareBufferManager::VertexDeclarationKey::VertexDeclarationKey(const Vector<VertexElement>& elements)
        :elements(elements)
    { }


    size_t HardwareBufferManager::VertexDeclarationKey::HashFunction::operator()(const VertexDeclarationKey& v) const
    {
        size_t hash = 0;
        for (auto& entry : v.elements)
        {
            te_hash_combine(hash, VertexElement::GetHash(entry));
        }

        return hash;
    }

    bool HardwareBufferManager::VertexDeclarationKey::EqualFunction::operator()(const VertexDeclarationKey& lhs,
        const VertexDeclarationKey& rhs) const
    {
        if (lhs.elements.size() != rhs.elements.size())
            return false;

        size_t numElements = lhs.elements.size();
        auto iterLeft = lhs.elements.begin();
        auto iterRight = rhs.elements.begin();
        for (size_t i = 0; i < numElements; i++)
        {
            if (*iterLeft != *iterRight)
                return false;

            ++iterLeft;
            ++iterRight;
        }

        return true;
    }

    SPtr<IndexBuffer> HardwareBufferManager::CreateIndexBuffer(const INDEX_BUFFER_DESC& desc,
        GpuDeviceFlags deviceMask)
    {
        SPtr<IndexBuffer> ibuf = CreateIndexBufferInternal(desc, deviceMask);
        ibuf->Initialize();
        return ibuf;

    }

    SPtr<VertexBuffer> HardwareBufferManager::CreateVertexBuffer(const VERTEX_BUFFER_DESC& desc,
        GpuDeviceFlags deviceMask)
    {
        SPtr<VertexBuffer> vbuf = CreateVertexBufferInternal(desc, deviceMask);
        vbuf->Initialize();
        return vbuf;
    }

    SPtr<VertexDeclaration> HardwareBufferManager::CreateVertexDeclaration(const SPtr<VertexDataDesc>& desc,
        GpuDeviceFlags deviceMask)
    {
        Vector<VertexElement> elements = desc->CreateElements();

        return CreateVertexDeclaration(elements, deviceMask);
    }

    SPtr<VertexDeclaration> HardwareBufferManager::CreateVertexDeclaration(const Vector<VertexElement>& elements,
        GpuDeviceFlags deviceMask)
    {
        VertexDeclarationKey key(elements);

        auto iterFind = _cachedDeclarations.find(key);
        if (iterFind != _cachedDeclarations.end())
        {
            return iterFind->second;
        }

        SPtr<VertexDeclaration> declPtr = createVertexDeclarationInternal(elements, deviceMask);
        declPtr->Initialize();

        _cachedDeclarations[key] = declPtr;
        return declPtr;
    }

    SPtr<VertexDeclaration> HardwareBufferManager::createVertexDeclarationInternal(
        const Vector<VertexElement>& elements, GpuDeviceFlags deviceMask)
    {
        VertexDeclaration* decl = new (te_allocate<VertexDeclaration>()) VertexDeclaration(elements, deviceMask);

        SPtr<VertexDeclaration> ret = te_shared_ptr<VertexDeclaration>(decl);
        ret->SetThisPtr(ret);

        return ret;
    }

    SPtr<GpuParamBlockBuffer> HardwareBufferManager::CreateGpuParamBlockBuffer(UINT32 size,
        GpuBufferUsage usage, GpuDeviceFlags deviceMask)
    {
        SPtr<GpuParamBlockBuffer> paramBlockPtr = CreateGpuParamBlockBufferInternal(size, usage, deviceMask);
        paramBlockPtr->Initialize();

        return paramBlockPtr;
    }

    SPtr<GpuParams> HardwareBufferManager::CreateGpuParams(const SPtr<GpuPipelineParamInfo>& paramInfo,
        GpuDeviceFlags deviceMask)
    {
        SPtr<GpuParams> params = CreateGpuParamsInternal(paramInfo, deviceMask);
        params->Initialize();

        return params;
    }

    SPtr<GpuParams> HardwareBufferManager::CreateGpuParamsInternal(
        const SPtr<GpuPipelineParamInfo>& paramInfo, GpuDeviceFlags deviceMask)
    {
        GpuParams* params = new (te_allocate<GpuParams>()) GpuParams(paramInfo, deviceMask);
        SPtr<GpuParams> paramsPtr = te_core_ptr<GpuParams>(params);
        paramsPtr->SetThisPtr(paramsPtr);

        return paramsPtr;
    }
}
