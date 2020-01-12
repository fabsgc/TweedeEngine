#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"
#include "Math/TeBounds.h"
#include "RenderAPI/TeCommonTypes.h"
#include "RenderAPI/TeVertexDataDesc.h"
#include "RenderAPI/TeSubMesh.h"

namespace te
{
	/**
	 * Planned usage for the mesh. These options usually affect performance and you should specify static if you don't plan
	 * on modifying the mesh often, otherwise specify dynamic.
	 */
	enum MeshUsage
	{
		/** Specify for a mesh that is not often updated from the CPU. */
		MU_STATIC = 1 << 0,

		/** Specify for a mesh that is often updated from the CPU. */
		MU_DYNAMIC = 1 << 1,
		/**
		 * All mesh data will also be cached in CPU memory, making it available for fast read access from the CPU. Can be
		 * combined with other usage flags.
		 */
		MU_CPUCACHED = 0x1000
	};

	/** Descriptor object used for creation of a new Mesh object. */
	struct TE_CORE_EXPORT MESH_DESC
	{
		MESH_DESC() { }

		/** Number of vertices in the mesh. */
		UINT32 NumVertices = 0;

		/** Number of indices in the mesh. */
		UINT32 NumIndices = 0;

		/**
		 * Vertex description structure that describes how are vertices organized in the vertex buffer. When binding a mesh
		 * to the pipeline you must ensure vertex description at least partially matches the input description of the
		 * currently bound vertex GPU program.
		 */
		SPtr<VertexDataDesc> VertexDesc;

		/**
		 * Defines how are indices separated into sub-meshes, and how are those sub-meshes rendered. Sub-meshes may be
		 * rendered independently.
		 */
		Vector<SubMesh> SubMeshes;

		/** Optimizes performance depending on planned usage of the mesh. */
		INT32 Usage = MU_STATIC;

		/**
		 * Size of indices, use smaller size for better performance, however be careful not to go over the number of
		 * vertices limited by the size.
		 */
		IndexType IndexType = IT_32BIT;

		static MESH_DESC DEFAULT;

		// TODO
	};

	/** Properties of a Mesh. Shared between sim and core thread versions of a Mesh. */
	class TE_CORE_EXPORT MeshProperties
	{
	public:
		MeshProperties();
		MeshProperties(UINT32 numVertices, UINT32 numIndices, DrawOperationType drawOp);
		MeshProperties(UINT32 numVertices, UINT32 numIndices, const Vector<SubMesh>& subMeshes);

		/**
		 * Retrieves a sub-mesh containing data used for rendering a certain portion of this mesh. If no sub-meshes are
		 * specified manually a special sub-mesh containing all indices is returned.
		 */
		const SubMesh& GetSubMesh(UINT32 subMeshIdx = 0) const;

		/** Retrieves a total number of sub-meshes in this mesh. */
		UINT32 GetNumSubMeshes() const;

		/**	Returns maximum number of vertices the mesh may store. */
		UINT32 GetNumVertices() const { return _numVertices; }

		/**	Returns maximum number of indices the mesh may store. */
		UINT32 GetNumIndices() const { return _numIndices; }

		/**	Returns bounds of the geometry contained in the vertex buffers for all sub-meshes. */
		const Bounds& GetBounds() const { return _bounds; }

	protected:
		friend class Mesh;

		Vector<SubMesh> _subMeshes;
		UINT32 _numVertices;
		UINT32 _numIndices;
		Bounds _bounds;

		// TODO
	};

	class TE_CORE_EXPORT Mesh : public Resource
	{
	public:
        ~Mesh();

		/** Returns a structure that describes how are the vertices stored in the mesh's vertex buffer. */
		SPtr<VertexDataDesc> GetVertexDesc() const;

		/**
		 * Creates a new empty mesh. Created mesh will have no sub-meshes.
		 *
		 * @param[in]	numVertices		Number of vertices in the mesh.
		 * @param[in]	numIndices		Number of indices in the mesh.
		 * @param[in]	vertexDesc		Vertex description structure that describes how are vertices organized in the
		 *								vertex buffer. When binding a mesh to the pipeline you must ensure vertex
		 *								description at least partially matches the input description of the currently bound
		 *								vertex GPU program.
		 * @param[in]	usage			Optimizes performance depending on planned usage of the mesh.
		 * @param[in]	drawOp			Determines how should the provided indices be interpreted by the pipeline. Default
		 *								option is a triangle list, where three indices represent a single triangle.
		 * @param[in]	indexType		Size of indices, use smaller size for better performance, however be careful not to
		 *								go over the number of vertices limited by the size.
		 */
		static HMesh Create(UINT32 numVertices, UINT32 numIndices, const SPtr<VertexDataDesc>& vertexDesc,
			int usage = MU_STATIC, DrawOperationType drawOp = DOT_TRIANGLE_LIST, IndexType indexType = IT_32BIT);

		/**
		 * Creates a new empty mesh.
		 *
		 * @param[in]	desc	Descriptor containing the properties of the mesh to create.
		 */
		static HMesh Create(const MESH_DESC& desc);

		/**
		 * @copydoc	create(const MESH_DESC&)
		 *
		 * @note	Internal method. Use create() for normal use.
		 */
		static SPtr<Mesh> _createPtr(const MESH_DESC& desc);

		/**
		 * Creates a new empty and uninitialized mesh. You will need to manually initialize the mesh before using it.
		 *	
		 * @note	This should only be used for special cases like serialization and is not meant for normal use.
		 */
		static SPtr<Mesh> CreateEmpty();

		/** @copydoc CoreObject::Initialize */
		void Initialize() override;

	protected:
		friend class MeshManager;

		Mesh(const MESH_DESC& desc);

		/**	Returns properties that contain information about the mesh. */
		const MeshProperties& GetProperties() const { return _properties; }

	private:
        Mesh();

		MeshProperties _properties;
		SPtr<VertexDataDesc> _vertexDesc;
		int _usage;
		IndexType _indexType;

		// TODO
    };
}
