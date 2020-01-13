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

    Mesh::Mesh(const MESH_DESC& desc)
		: _properties(desc.NumVertices, desc.NumIndices, desc.SubMeshes)
        , _vertexDesc(desc.VertexDesc)
        , _usage(desc.Usage)
		, _indexType(desc.IndType)
	{ }

    Mesh::Mesh()
		: _properties(0, 0, DOT_TRIANGLE_LIST)
	{ }

    Mesh::~Mesh()
	{
		_vertexDesc = nullptr;
	}

    void Mesh::Initialize()
	{ 
        _size = 8; // TODO
        bool isDynamic = (_usage & MU_DYNAMIC) != 0;
		int usage = isDynamic ? GBU_DYNAMIC : GBU_STATIC;

        Resource::Initialize();
        CoreObject::Initialize();
    }

    SPtr<VertexDataDesc> Mesh::GetVertexDesc() const
	{
		return _vertexDesc;
	}

    HMesh Mesh::Create(UINT32 numVertices, UINT32 numIndices, const SPtr<VertexDataDesc>& vertexDesc,
		int usage, DrawOperationType drawOp, IndexType indexType)
	{
		MESH_DESC desc;
		desc.NumVertices = numVertices;
		desc.NumIndices = numIndices;
		desc.VertexDesc = vertexDesc;
		desc.Usage = usage;
		desc.SubMeshes.push_back(SubMesh(0, numIndices, drawOp));
		desc.IndType = indexType;

		SPtr<Mesh> meshPtr = _createPtr(desc);
		return static_resource_cast<Mesh>(ResourceManager()._createResourceHandle(meshPtr));
	}

    HMesh Mesh::Create(const MESH_DESC& desc)
	{
		SPtr<Mesh> meshPtr = _createPtr(desc);
		return static_resource_cast<Mesh>(ResourceManager()._createResourceHandle(meshPtr));
	}

    SPtr<Mesh> Mesh::_createPtr(const MESH_DESC& desc)
	{
		SPtr<Mesh> mesh = te_core_ptr<Mesh>(new (te_allocate<Mesh>()) Mesh(desc));
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
}
