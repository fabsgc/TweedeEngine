#pragma once

#include "TeCorePrerequisites.h"
#include "Importer/TeImportOptions.h"
#include "Serialization/TeSerializable.h"
#include "Animation/TeAnimationClip.h"

namespace te
{
    /** Controls what type of collision mesh should be imported during mesh import. */
    enum class CollisionMeshType
    {
        None, /**< No collision mesh will be imported. */
        Triangle, /**< Normal triangle mesh will be imported. */
        Convex /**< A convex hull will be generated from the source mesh. */
    };

    /** Information about how to split an AnimationClip into multiple separate clips. */
    struct TE_CORE_EXPORT AnimationSplitInfo : Serializable
    {
        AnimationSplitInfo()
            : Serializable(TID_AnimationSplitInfo)
        { }

        AnimationSplitInfo(const String & name, UINT32 startFrame, UINT32 endFrame, bool isAdditive = false)
            : Serializable(TID_AnimationSplitInfo)
            , Name(name)
            , StartFrame(startFrame)
            , EndFrame(endFrame)
            , IsAdditive(isAdditive)
        { }

        String Name;
        UINT32 StartFrame = 0;
        UINT32 EndFrame = 0;
        bool IsAdditive = false;
    };

    /** A set of animation events that will be added to an animation clip during animation import. */
    struct TE_CORE_EXPORT ImportedAnimationEvents : Serializable
    {
        ImportedAnimationEvents()
            : Serializable(TID_ImportedAnimationEvents)
        { }

        String Name;
        Vector<AnimationEvent> Events;
    };

    /** Contains import options you may use to control how is a Mesh imported. */
    class TE_CORE_EXPORT MeshImportOptions : public ImportOptions
    {
    public:
        MeshImportOptions();

        /** Determines whether the texture data is also stored in CPU memory. */
        bool CpuCached = false;

        /** Determines should mesh normals be imported if available. */
        bool ImportNormals = true;

        /** Determines should mesh tangents and bitangents be imported if available. */
        bool ImportTangents = true;

        /** Determines should mesh skin data like bone weights, indices and bind poses be imported if available. */
        bool ImportSkin = false;

        /**	Determines should mesh blend shapes be imported if available. */
        bool ImportBlendShapes = false;

        /**	Determines should animation clips be imported if available. */
        bool ImportAnimations = false;

        /** Determines should vertex colors be imported if available */
        bool ImportVertexColors = false;

        /** Determines should be re-generated during loading */
        bool ForceGenNormals = false;

        /** Determines should normals smoothed during loading */
        bool GenSmoothNormals = false;

        /**
         * Enables or disables keyframe reduction. Keyframe reduction will reduce the number of key-frames in an animation
         * clip by removing identical keyframes, and therefore reducing the size of the clip.
         */
        bool ReduceKeyFrames = true;

        /** Determine if we need to flip UV mapping when importing object */
        bool FplitUV = false;

        /** Determine if we need to change coordinate system */
        bool LeftHanded = false;

        /** Determine if we need to flip winding order in order to adjusts the output face winding order to be CW */
        bool FlipWinding = false;

        /** For FBX format, scale is in centimeters instead of meters, we must scale matrices using scale factor */
        bool ScaleSystemUnit = false;

        /** Uniformly scales the imported mesh by the specified value. */
        float ScaleFactor = 1.0f;

        /** Determines if we need to set material properties for this mesh during import */
        bool ImportMaterials = true;

        /**
         * Enables or disables import of root motion curves. When enabled, any animation curves in imported animations
         * affecting the root bone will be available through a set of separate curves in AnimationClip, and they won't be
         * evaluated through normal animation process. Instead it is expected that the user evaluates the curves manually
         * and applies them as required.
         */
        bool ImportRootMotion = false;

        /**
         * Animation split infos that determine how will the source animation clip be split. If no splits are present the
         * data will be imported as one clip, but if splits are present the data will be split according to the split infos.
         * Split infos only affect the primary animation clip, other clips will not be split.
         */
        Vector<AnimationSplitInfo> AnimationSplits;

        /** Set of events that will be added to the animation clip, if animation import is enabled. */
        Vector<ImportedAnimationEvents> AnimationEvents;

        /**
         * Determines what type (if any) of collision mesh should be imported. If enabled the collision mesh will be
         * available as a sub-resource returned by the importer (along with the normal mesh).
         */
        CollisionMeshType CollisionType = CollisionMeshType::None;

        /** Creates a new import options object that allows you to customize how are Meshs imported. */
        static SPtr<MeshImportOptions> Create();
    };
}
