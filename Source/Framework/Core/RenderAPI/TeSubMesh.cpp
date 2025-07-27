#include "RenderAPI/TeSubMesh.h"

#include "Resources/TeResourceManager.h"
#include "Serialization/TeUtility.h"

namespace te
{
    SubMesh::SubMesh(UINT32 indexOffset, UINT32 indexCount, DrawOperationType drawOp, const String& materialName, const String& name)
        : IndexOffset(indexOffset)
        , IndexCount(indexCount)
        , DrawOp(drawOp)
        , MaterialName(materialName)
        , Name(name)
    { }

    void SubMesh::ExportJson(nlohmann::json& document) const
    {
        document["indexOffset"] = IndexOffset;
        document["indexCount"] = IndexCount;
        document["drawOp"] = DrawOp;
        document["materialName"] = MaterialName;
        document["name"] = Name;
        document["material"] = Mat.IsLoaded() ? serialization::GetResourceName(Mat.Get()) : "";
        MatProperties.ExportJson(document["materialProperties"]);
        MatTextures.ExportJson(document["materialTextures"]);
        SubMeshBounds.ExportJson(document["subMeshBounds"]);
    }

    SubMesh SubMesh::ImportJson(const nlohmann::json& document)
    {
        SubMesh subMesh;

        subMesh.IndexOffset = document["indexOffset"].get<uint32_t>();
        subMesh.IndexCount = document["indexCount"].get<uint32_t>();
        subMesh.DrawOp = static_cast<DrawOperationType>(document["drawOp"].get<uint32_t>());
        subMesh.MaterialName = document["materialName"].get<String>();
        subMesh.Name = document["name"].get<String>();
        subMesh.Mat = static_resource_cast<Material>(gResourceManager().Get(serialization::GetResourceUUID(document["material"].get<String>())));
        subMesh.MatProperties = MaterialProperties::ImportJson(document["materialProperties"]);
        subMesh.MatTextures = MaterialTextures::ImportJson(document["materialTextures"]);
        subMesh.SubMeshBounds = Bounds::ImportJson(document["subMeshBounds"]);

        return subMesh;
    }
}
