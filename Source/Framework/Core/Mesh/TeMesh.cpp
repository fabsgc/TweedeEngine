#include "Mesh/TeMesh.h"
#include "Resources/TeResourceManager.h"

namespace te
{
    MESH_DESC MESH_DESC::DEFAULT = MESH_DESC();

    MeshProperties::MeshProperties()
        : _numVertices(0) 
        , _numIndices(0)
    {
        _subMeshes.reserve(10);
    }

    MeshProperties::MeshProperties(UINT32 numVertices, UINT32 numIndices, DrawOperationType drawOp)
        : _numVertices(numVertices)
        , _numIndices(numIndices)
    {
        _subMeshes.push_back(SubMesh(0, numIndices, drawOp));
    }

    MeshProperties::MeshProperties(UINT32 numVertices, UINT32 numIndices, const Vector<SubMesh>& subMeshes)
        : _numVertices(numVertices)
        , _numIndices(numIndices)
    {
        _subMeshes = subMeshes;
    }

    const SubMesh& MeshProperties::GetSubMesh(UINT32 subMeshIdx) const
    {
        if (subMeshIdx >= _subMeshes.size())
        {
            TE_ASSERT_ERROR(false, "Invalid sub-mesh index ("+ ToString(subMeshIdx) + "). Number of sub-meshes available: " + ToString((int)_subMeshes.size()), __FILE__, __LINE__);
        }

        return _subMeshes[subMeshIdx];
    }

    UINT32 MeshProperties::GetNumSubMeshes() const
    {
        return (UINT32)_subMeshes.size();
    }

    Mesh::Mesh()
        : _properties(0, 0, DOT_TRIANGLE_LIST)
        , _CPUData(nullptr)
        , _vertexData(nullptr)
        , _indexBuffer(nullptr)
        , _vertexDesc(nullptr)
        , _tempInitialMeshData(nullptr)
        , _usage(GBU_STATIC)
        , _indexType(IT_32BIT)
        , _deviceMask(GDF_DEFAULT)
    { }

    Mesh::Mesh(const MESH_DESC& desc, GpuDeviceFlags deviceMask)
        : _properties(desc.NumVertices, desc.NumIndices, desc.SubMeshes)
        , _CPUData(nullptr)
        , _vertexData(nullptr)
        , _indexBuffer(nullptr)
        , _vertexDesc(desc.VertexDesc)
        , _tempInitialMeshData(nullptr)
        , _usage(desc.Usage)
        , _indexType(desc.IndType)
        , _deviceMask(deviceMask)
    { }

    Mesh::Mesh(const SPtr<MeshData>& initialMeshData, const MESH_DESC& desc, GpuDeviceFlags deviceMask)
        : _properties(initialMeshData->GetNumVertices(), initialMeshData->GetNumIndices(), desc.SubMeshes)
        , _CPUData(initialMeshData)
        , _vertexData(nullptr)
        , _indexBuffer(nullptr)
        , _vertexDesc(initialMeshData->GetVertexDesc())
        , _tempInitialMeshData(initialMeshData)
        , _usage(desc.Usage)
        , _indexType(initialMeshData->GetIndexType())
        , _deviceMask(deviceMask)
    { }

    Mesh::~Mesh()
    {
        _vertexData = nullptr;
        _vertexDesc = nullptr;
        _tempInitialMeshData = nullptr;

        if (_indexBuffer != nullptr)
        {
            _indexBuffer->Destroy();
            _indexBuffer = nullptr;
        }
    }

    void Mesh::Initialize()
    { 
        if (_CPUData != nullptr)
        {
            UpdateBounds(*_CPUData);
        }

        CoreObjectManager::Instance().RegisterObject(this);

        if ((_usage & MU_CPUCACHED) != 0 && _CPUData == nullptr)
        {
            CreateCPUBuffer();
        }

        bool isDynamic = (_usage & MU_DYNAMIC) != 0;
        int usage = isDynamic ? GBU_DYNAMIC : GBU_STATIC;

        INDEX_BUFFER_DESC ibDesc;
        ibDesc.Type = _indexType;
        ibDesc.NumIndices = _properties._numIndices;
        ibDesc.Usage = (GpuBufferUsage)usage;

        _indexBuffer = IndexBuffer::Create(ibDesc, _deviceMask);

        _vertexData = te_shared_ptr_new<VertexData>();
        _vertexData->vertexCount = _properties._numVertices;
        _vertexData->vertexDeclaration = VertexDeclaration::Create(_vertexDesc, _deviceMask);

        for (UINT32 i = 0; i <= _vertexDesc->GetMaxStreamIdx(); i++)
        {
            if (!_vertexDesc->HasStream(i))
                continue;

            VERTEX_BUFFER_DESC vbDesc;
            vbDesc.VertexSize = _vertexData->vertexDeclaration->GetProperties().GetVertexSize(i);
            vbDesc.NumVerts = _vertexData->vertexCount;
            vbDesc.Usage = (GpuBufferUsage)usage;

            SPtr<VertexBuffer> vertexBuffer = VertexBuffer::Create(vbDesc, _deviceMask);
            _vertexData->SetBuffer(i, vertexBuffer);
        }

        // TODO Low priority - DX11 (and maybe OpenGL)? allow an optimization that allows you to set
        // buffer data upon buffer construction, instead of setting it in a second step like I do here
        if (_tempInitialMeshData != nullptr)
        {
            WriteData(*_tempInitialMeshData, isDynamic);
            _tempInitialMeshData = nullptr;
        }

        Resource::Initialize();
        CoreObject::Initialize();
    }

    void Mesh::UpdateBounds(const MeshData& meshData)
    {
        _properties._bounds = meshData.CalculateBounds();
    }

    void Mesh::UpdateCPUBuffer(UINT32 subresourceIdx, const MeshData& meshData)
    {
        if ((_usage & MU_CPUCACHED) == 0)
        {
            return;
        }

        if (subresourceIdx > 0)
        {
            TE_DEBUG("Invalid subresource index: { " + ToString(subresourceIdx) + "}. Supported range: 0 .. 1.", __FILE__, __LINE__);
            return;
        }

        if (meshData.GetNumIndices() != _properties.GetNumIndices() ||
            meshData.GetNumVertices() != _properties.GetNumVertices() ||
            meshData.GetIndexType() != _indexType ||
            meshData.GetVertexDesc()->GetVertexStride() != _vertexDesc->GetVertexStride())
        {
            TE_DEBUG("Provided buffer is not of valid dimensions or format in order to update this mesh.", __FILE__, __LINE__);
            return;
        }

        if (_CPUData->GetSize() != meshData.GetSize())
        {
            TE_ASSERT_ERROR(false, "Buffer sizes don't match.", __FILE__, __LINE__);
        }

        UINT8* dest = _CPUData->GetData();
        UINT8* src = meshData.GetData();

        memcpy(dest, src, meshData.GetSize());
    }

    void Mesh::CreateCPUBuffer()
    {
        _CPUData = AllocateBuffer();
    }

    SPtr<MeshData> Mesh::AllocateBuffer() const
    {
        SPtr<MeshData> meshData = te_shared_ptr_new<MeshData>(_properties._numVertices, _properties._numIndices,
            _vertexDesc, _indexType);

        return meshData;
    }

    SPtr<VertexDataDesc> Mesh::GetVertexDesc() const
    {
        return _vertexDesc;
    }

    /** Get vertex data used for rendering. */
    SPtr<VertexData> Mesh::GetVertexData() const
    {
        return _vertexData;
    }

    /** Get index data used for rendering. */
    SPtr<IndexBuffer> Mesh::GetIndexBuffer() const
    {
        return _indexBuffer;
    }

    HMesh Mesh::Create(const MESH_DESC& desc, GpuDeviceFlags deviceMask)
    {
        SPtr<Mesh> meshPtr = _createPtr(desc);
        return static_resource_cast<Mesh>(ResourceManager()._createResourceHandle(meshPtr));
    }

    HMesh Mesh::Create(const SPtr<MeshData>& initialMeshData, const MESH_DESC& desc, GpuDeviceFlags deviceMask)
    {
        SPtr<Mesh> meshPtr = _createPtr(initialMeshData, desc);
        return static_resource_cast<Mesh>(gResourceManager()._createResourceHandle(meshPtr));
    }

    HMesh Mesh::Create(const SPtr<MeshData>& initialMeshData, int usage, DrawOperationType drawOp, GpuDeviceFlags deviceMask)
    {
        SPtr<Mesh> meshPtr = _createPtr(initialMeshData, usage, drawOp);
        return static_resource_cast<Mesh>(gResourceManager()._createResourceHandle(meshPtr));
    }

    HMesh Mesh::Create(UINT32 numVertices, UINT32 numIndices, const SPtr<VertexDataDesc>& vertexDesc,
        int usage, DrawOperationType drawOp, IndexType indexType, GpuDeviceFlags deviceMask)
    {
        MESH_DESC desc;
        desc.NumVertices = numVertices;
        desc.NumIndices = numIndices;
        desc.VertexDesc = vertexDesc;
        desc.Usage = usage;
        desc.SubMeshes.push_back(SubMesh(0, numIndices, drawOp));
        desc.IndType = indexType;

        SPtr<Mesh> meshPtr = _createPtr(desc, deviceMask);
        return static_resource_cast<Mesh>(ResourceManager()._createResourceHandle(meshPtr));
    }

    SPtr<Mesh> Mesh::_createPtr(const MESH_DESC& desc, GpuDeviceFlags deviceMask)
    {
        SPtr<Mesh> mesh = te_core_ptr<Mesh>(new (te_allocate<Mesh>()) Mesh(desc, deviceMask));
        mesh->SetThisPtr(mesh);
        mesh->Initialize();

        return mesh;
    }

    SPtr<Mesh> Mesh::_createPtr(const SPtr<MeshData>& initialMeshData, const MESH_DESC& desc, GpuDeviceFlags deviceMask)
    {
        SPtr<Mesh> mesh = te_core_ptr<Mesh>(new (te_allocate<Mesh>()) Mesh(initialMeshData, desc, deviceMask));
        mesh->SetThisPtr(mesh);
        mesh->Initialize();

        return mesh;
    }

    SPtr<Mesh> Mesh::_createPtr(const SPtr<MeshData>& initialMeshData, int usage, DrawOperationType drawOp, GpuDeviceFlags deviceMask)
    {
        MESH_DESC desc;
        desc.Usage = usage;
        desc.SubMeshes.push_back(SubMesh(0, initialMeshData->GetNumIndices(), drawOp));

        SPtr<Mesh> mesh = te_core_ptr<Mesh>(new (te_allocate<Mesh>()) Mesh(initialMeshData, desc, deviceMask));
        mesh->SetThisPtr(mesh);
        mesh->Initialize();

        return mesh;
    }

    SPtr<Mesh> Mesh::CreateEmpty()
    {
        SPtr<Mesh> mesh = te_core_ptr<Mesh>(new (te_allocate<Mesh>()) Mesh());
        mesh->SetThisPtr(mesh);

        return mesh;
    }

    void Mesh::WriteData(const MeshData& meshData, bool discardEntireBuffer, bool performUpdateBounds, UINT32 queueIdx)
    {
        if (discardEntireBuffer)
        {
            if ((_usage & MU_STATIC) != 0)
            {
                TE_DEBUG("Buffer discard is enabled but buffer was not created as dynamic. Disabling discard.", __FILE__, __LINE__);
                discardEntireBuffer = false;
            }
        }
        else
        {
            if ((_usage & MU_DYNAMIC) != 0)
            {
                TE_DEBUG("Buffer discard is not enabled but buffer was created as dynamic. Enabling discard.", __FILE__, __LINE__);
                discardEntireBuffer = true;
            }
        }

        // Indices
        const IndexBufferProperties& ibProps = _indexBuffer->GetProperties();

        UINT32 indicesSize = meshData.GetIndexBufferSize();
        UINT8* srcIdxData = meshData.GetIndexData();

        if (meshData.GetIndexElementSize() != ibProps.GetIndexSize())
        {
            TE_DEBUG("Provided index size doesn't match meshes index size. Needed: {" + ToString(ibProps.GetIndexSize()) + "}. " +
                "Got: {" + ToString(meshData.GetIndexElementSize()) + "}", __FILE__, __LINE__);

            return;
        }

        if (indicesSize > _indexBuffer->GetSize())
        {
            indicesSize = _indexBuffer->GetSize();
            TE_DEBUG("Index buffer values are being written out of valid range.", __FILE__, __LINE__);
        }

        _indexBuffer->WriteData(0, indicesSize, srcIdxData, discardEntireBuffer ? BWT_DISCARD : BWT_NORMAL, queueIdx);

        // Vertices
        for (UINT32 i = 0; i <= _vertexDesc->GetMaxStreamIdx(); i++)
        {
            if (!_vertexDesc->HasStream(i))
            {
                continue;
            }

            if (!meshData.GetVertexDesc()->HasStream(i))
                continue;

            // Ensure both have the same sized vertices
            UINT32 myVertSize = _vertexDesc->GetVertexStride(i);
            UINT32 otherVertSize = meshData.GetVertexDesc()->GetVertexStride(i);
            if (myVertSize != otherVertSize)
            {
                TE_DEBUG("Provided vertex size for stream {" + ToString(i) + "} doesn't match meshes vertex size. "
                    "Needed: {" + ToString(myVertSize) + "}. Got: {" + ToString(otherVertSize) + "}", __FILE__, __LINE__);

                continue;
            }

            SPtr<VertexBuffer> vertexBuffer = _vertexData->GetBuffer(i);

            UINT32 bufferSize = meshData.GetStreamSize(i);
            UINT8* srcVertBufferData = meshData.GetStreamData(i);

            if (bufferSize > vertexBuffer->GetSize())
            {
                bufferSize = vertexBuffer->GetSize();
                TE_DEBUG("Vertex buffer values for stream \"{" + ToString(i) + "}\" are being written out of valid range.", __FILE__, __LINE__);
            }

            vertexBuffer->WriteData(0, bufferSize, srcVertBufferData, discardEntireBuffer ? BWT_DISCARD : BWT_NORMAL, queueIdx);
        }

        if (performUpdateBounds)
        {
            UpdateBounds(meshData);
        }
    }

    void Mesh::ReadData(MeshData& meshData, UINT32 deviceIdx, UINT32 queueIdx)
    {
        IndexType indexType = IT_32BIT;
        if (_indexBuffer)
        {
            indexType = _indexBuffer->GetProperties().GetType();
        }

        if (_indexBuffer)
        {
            const IndexBufferProperties& ibProps = _indexBuffer->GetProperties();

            if (meshData.GetIndexElementSize() != ibProps.GetIndexSize())
            {
                TE_DEBUG("Provided index size doesn't match meshes index size. Needed: {" + ToString(ibProps.GetIndexSize()) + "}. " + 
                         "Got: {" + ToString(meshData.GetIndexElementSize()) + "}", __FILE__, __LINE__);
                return;
            }

            UINT8* idxData = static_cast<UINT8*>(_indexBuffer->Lock(GBL_READ_ONLY, deviceIdx, queueIdx));
            UINT32 idxElemSize = ibProps.GetIndexSize();

            UINT8* indices = nullptr;

            if (indexType == IT_16BIT)
                indices = (UINT8*)meshData.GetIndices16();
            else
                indices = (UINT8*)meshData.GetIndices32();

            UINT32 numIndicesToCopy = std::min(_properties._numIndices, meshData.GetNumIndices());

            UINT32 indicesSize = numIndicesToCopy * idxElemSize;
            if (indicesSize > meshData.GetIndexBufferSize())
            {
                TE_DEBUG("Provided buffer doesn't have enough space to store mesh indices.", __FILE__, __LINE__);
                return;
            }

            memcpy(indices, idxData, numIndicesToCopy * idxElemSize);

            _indexBuffer->Unlock();
        }

        if (_vertexData)
        {
            auto vertexBuffers = _vertexData->GetBuffers();

            UINT32 streamIdx = 0;
            for (auto iter = vertexBuffers.begin(); iter != vertexBuffers.end(); ++iter)
            {
                if (!meshData.GetVertexDesc()->HasStream(streamIdx))
                    continue;

                SPtr<VertexBuffer> vertexBuffer = iter->second;
                const VertexBufferProperties& vbProps = vertexBuffer->GetProperties();

                // Ensure both have the same sized vertices
                UINT32 myVertSize = _vertexDesc->GetVertexStride(streamIdx);
                UINT32 otherVertSize = meshData.GetVertexDesc()->GetVertexStride(streamIdx);
                if (myVertSize != otherVertSize)
                {
                    TE_DEBUG("Provided vertex size for stream {" + ToString(streamIdx) + "} doesn't match meshes vertex size. "
                        "Needed: {" + ToString(myVertSize) + "}. Got: {" + ToString(otherVertSize) + "}", __FILE__, __LINE__);

                    continue;
                }

                UINT32 numVerticesToCopy = meshData.GetNumVertices();
                UINT32 bufferSize = vbProps.GetVertexSize() * numVerticesToCopy;

                if (bufferSize > vertexBuffer->GetSize())
                {
                    TE_DEBUG("Vertex buffer values for stream \"{" + ToString(streamIdx) + "}\" are being read out of valid range.", __FILE__, __LINE__);
                    continue;
                }

                UINT8* vertDataPtr = static_cast<UINT8*>(vertexBuffer->Lock(GBL_READ_ONLY, deviceIdx, queueIdx));

                UINT8* dest = meshData.GetStreamData(streamIdx);
                memcpy(dest, vertDataPtr, bufferSize);

                vertexBuffer->Unlock();

                streamIdx++;
            }
        }
    }
}
