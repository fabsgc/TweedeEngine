#include "TeD3D11InputLayoutManager.h"
#include "TeD3D11Mappings.h"
#include "TeD3D11RenderAPI.h"
#include "TeD3D11Device.h"
#include "TeD3D11Utility.h"
#include "TeD3D11GpuProgram.h"
#include "RenderAPI/TeHardwareBufferManager.h"
#include "Profiling/TeProfilerGPU.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(D3D11InputLayoutManager)

    size_t D3D11InputLayoutManager::HashFunc::operator()
        (const D3D11InputLayoutManager::VertexDeclarationKey& key) const
    {
        size_t hash = 0;
        te_hash_combine(hash, key.vertxDeclId);
        te_hash_combine(hash, key.vertexProgramId);

        return hash;
    }

    bool D3D11InputLayoutManager::EqualFunc::operator()
        (const D3D11InputLayoutManager::VertexDeclarationKey& a, const D3D11InputLayoutManager::VertexDeclarationKey& b) const

    {
        if (a.vertxDeclId != b.vertxDeclId)
            return false;

        if (a.vertexProgramId != b.vertexProgramId)
            return false;

        return true;
    }

    D3D11InputLayoutManager::~D3D11InputLayoutManager()
    {
        while (_inputLayoutMap.begin() != _inputLayoutMap.end())
        {
            auto firstElem = _inputLayoutMap.begin();

            SAFE_RELEASE(firstElem->second->InputLayout);
            te_delete(firstElem->second);

            _inputLayoutMap.erase(firstElem);

            TE_INC_PROFILER_GPU(ResDestroyed);
        }
    }

    ID3D11InputLayout* D3D11InputLayoutManager::RetrieveInputLayout(const SPtr<VertexDeclaration>& vertexShaderDecl,
        const SPtr<VertexDeclaration>& vertexBufferDecl, D3D11GpuProgram& vertexProgram)
    {
        VertexDeclarationKey pair;
        pair.vertxDeclId = vertexBufferDecl->GetId();
        pair.vertexProgramId = vertexProgram.GetProgramId();

        auto iterFind = _inputLayoutMap.find(pair);
        if (iterFind == _inputLayoutMap.end())
        {
            if (_inputLayoutMap.size() >= DECLARATION_BUFFER_SIZE)
            {
                RemoveLeastUsed(); // Prune so the buffer doesn't just infinitely grow
            }

            AddNewInputLayout(vertexShaderDecl, vertexBufferDecl, vertexProgram);

            iterFind = _inputLayoutMap.find(pair);

            if (iterFind == _inputLayoutMap.end()) // We failed to create input layout
            {
                return nullptr;
            }
        }

        iterFind->second->LastUsedIdx = ++_lastUsedCounter;
        return iterFind->second->InputLayout;
    }

    void D3D11InputLayoutManager::AddNewInputLayout(const SPtr<VertexDeclaration>& vertexShaderDecl,
        const SPtr<VertexDeclaration>& vertexBufferDecl, D3D11GpuProgram& vertexProgram)
    {
        const VertexDeclarationProperties& bufferDeclProps = vertexBufferDecl->GetProperties();
        const VertexDeclarationProperties& shaderDeclProps = vertexShaderDecl->GetProperties();

        Vector<D3D11_INPUT_ELEMENT_DESC> declElements;

        const Vector<VertexElement>& bufferElems = bufferDeclProps.GetElements();
        const Vector<VertexElement>& shaderElems = shaderDeclProps.GetElements();

        INT32 maxStreamIdx = -1;
        UINT currentByteOffset = 0;
        for (const auto& buffer : bufferElems)
        {
            declElements.push_back(D3D11_INPUT_ELEMENT_DESC());
            D3D11_INPUT_ELEMENT_DESC& elementDesc = declElements.back();

            elementDesc.SemanticName = D3D11Mappings::Get(buffer.GetSemantic());
            elementDesc.SemanticIndex = buffer.GetSemanticIdx();
            elementDesc.Format = D3D11Mappings::Get(buffer.GetType());
            elementDesc.InputSlot = buffer.GetStreamIdx();
            elementDesc.AlignedByteOffset = currentByteOffset;

            currentByteOffset += (D3D11Mappings::GetSize(D3D11Mappings::Get(buffer.GetType())) / 8);

            if (buffer.GetInstanceStepRate() == 0)
            {
                elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
                elementDesc.InstanceDataStepRate = 0;
            }
            else
            {
                elementDesc.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
                elementDesc.InstanceDataStepRate = buffer.GetInstanceStepRate();
            }

            maxStreamIdx = std::max(maxStreamIdx, (INT32)buffer.GetStreamIdx());
        }

        // Find elements missing in buffer and add a dummy stream for them
        for (const auto& shader : shaderElems)
        {
            bool foundElement = false;
            for (const auto& buffer : bufferElems)
            {
                if (shader.GetSemantic() == buffer.GetSemantic() && shader.GetSemanticIdx() == buffer.GetSemanticIdx())
                {
                    foundElement = true;
                    break;
                }
            }

            if (!foundElement)
            {
                declElements.push_back(D3D11_INPUT_ELEMENT_DESC());
                D3D11_INPUT_ELEMENT_DESC& elementDesc = declElements.back();

                elementDesc.SemanticName = D3D11Mappings::Get(shader.GetSemantic());
                elementDesc.SemanticIndex = shader.GetSemanticIdx();
                elementDesc.Format = D3D11Mappings::Get(shader.GetType());
                elementDesc.InputSlot = (UINT32)(maxStreamIdx + 1);
                elementDesc.AlignedByteOffset = 0;
                elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
                elementDesc.InstanceDataStepRate = 0;

                TE_DEBUG("Element " + ToString(ToWString(elementDesc.SemanticName)) + " not found");
            }
        }

        D3D11RenderAPI* d3d11rs = static_cast<D3D11RenderAPI*>(RenderAPI::InstancePtr());
        D3D11Device& device = d3d11rs->GetPrimaryDevice();

        const DataBlob& microcode = vertexProgram.GetMicroCode();

        InputLayoutEntry* newEntry = te_new<InputLayoutEntry>();
        newEntry->LastUsedIdx = ++_lastUsedCounter;
        newEntry->InputLayout = nullptr;
        HRESULT hr = device.GetD3D11Device()->CreateInputLayout(
            &declElements[0],
            (UINT32)declElements.size(),
            microcode.Data,
            microcode.Size,
            &newEntry->InputLayout);

        if (FAILED(hr) || device.HasError())
        {
            TE_ASSERT_ERROR(false, "Unable to set D3D11 vertex declaration" + device.GetErrorDescription());
        }

#if  TE_DEBUG_MODE == TE_DEBUG_ENABLED
        String gpuProgramDebugName = vertexProgram.GetDebugName();
        String debugName = !gpuProgramDebugName.empty() ? "[LAYOUT] " + gpuProgramDebugName : "[LAYOUT]";
        D3D11Utility::SetDebugName(newEntry->InputLayout, debugName.c_str(), debugName.size());
#endif

        // Create key and add to the layout map
        VertexDeclarationKey pair;
        pair.vertxDeclId = vertexBufferDecl->GetId();
        pair.vertexProgramId = vertexProgram.GetProgramId();

        _inputLayoutMap[pair] = newEntry;

        TE_INC_PROFILER_GPU(ResCreated);
    }

    void D3D11InputLayoutManager::RemoveLeastUsed()
    {
        if (!_warningShown)
        {
            TE_DEBUG("Input layout buffer is full, pruning last " + ToString(NUM_ELEMENTS_TO_PRUNE) + "elements. This is probably okay " +
                "unless you are creating a massive amount of input layouts as they will get re-created every frame. "
                "In that case you should increase the layout buffer size. This warning won't be shown again.");

            _warningShown = true;
        }

        Map<UINT32, VertexDeclarationKey> leastFrequentlyUsedMap;

        for (const auto& inputLayout : _inputLayoutMap)
        {
            leastFrequentlyUsedMap[inputLayout.second->LastUsedIdx] = inputLayout.first;
        }

        UINT32 elemsRemoved = 0;
        for (const auto& inputLayout : leastFrequentlyUsedMap)
        {
            auto inputLayoutIter = _inputLayoutMap.find(inputLayout.second);

            SAFE_RELEASE(inputLayoutIter->second->InputLayout);
            te_delete(inputLayoutIter->second);

            _inputLayoutMap.erase(inputLayoutIter);
            TE_INC_PROFILER_GPU(ResDestroyed);

            elemsRemoved++;
            if (elemsRemoved >= NUM_ELEMENTS_TO_PRUNE)
            {
                break;
            }
        }
    }
}
