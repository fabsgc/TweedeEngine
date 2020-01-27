#include "TeObjectImportData.h"

namespace te
{
    AssimpImportNode::~AssimpImportNode()
    {
        for (auto& child : Children)
            te_delete(child);
    }

    AssimpImportScene::~AssimpImportScene()
    {
        if (RootNode != nullptr)
            te_delete(RootNode);

        for (auto& mesh : Meshes)
            te_delete(mesh);
    }
}
