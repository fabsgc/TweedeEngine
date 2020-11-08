#include "TeSkeleton.h"

namespace te
{ 
    LocalSkeletonPose::LocalSkeletonPose(UINT32 numBones, bool individualOverride)
        : NumBones(numBones)
    {
        const UINT32 overridesPerBone = individualOverride ? 3 : 1;

        UINT32 elementSize = sizeof(Vector3) * 2 + sizeof(Quaternion) + sizeof(bool) * overridesPerBone;
        UINT8* buffer = (UINT8*)te_allocate(elementSize * numBones);

        Positions = (Vector3*)buffer;
        buffer += sizeof(Vector3) * numBones;

        Rotations = (Quaternion*)buffer;
        buffer += sizeof(Quaternion) * numBones;

        Scales = (Vector3*)buffer;
        buffer += sizeof(Vector3) * numBones;

        HasOverride = (bool*)buffer;
    }

    LocalSkeletonPose::LocalSkeletonPose(UINT32 numPos, UINT32 numRot, UINT32 numScale)
    {
        UINT32 bufferSize = sizeof(Vector3) * numPos + sizeof(Quaternion) * numRot + sizeof(Vector3) * numScale;
        UINT8* buffer = (UINT8*)te_allocate(bufferSize);

        Positions = (Vector3*)buffer;
        buffer += sizeof(Vector3) * numPos;

        Rotations = (Quaternion*)buffer;
        buffer += sizeof(Quaternion) * numRot;

        Scales = (Vector3*)buffer;
    }

    LocalSkeletonPose::LocalSkeletonPose(LocalSkeletonPose&& other)
        : Positions{ std::exchange(other.Positions, nullptr) }
        , Rotations{ std::exchange(other.Rotations, nullptr) }
        , Scales{ std::exchange(other.Scales, nullptr) }
        , HasOverride{ std::exchange(other.HasOverride, nullptr) }
        , NumBones(std::exchange(other.NumBones, 0))
    { }

    LocalSkeletonPose::~LocalSkeletonPose()
    {
        if (Positions != nullptr)
            te_free(Positions);
    }

    LocalSkeletonPose& LocalSkeletonPose::operator=(LocalSkeletonPose&& other)
    {
        if (this != &other)
        {
            if (Positions != nullptr)
                te_free(Positions);

            Positions = std::exchange(other.Positions, nullptr);
            Rotations = std::exchange(other.Rotations, nullptr);
            Scales = std::exchange(other.Scales, nullptr);
            HasOverride = std::exchange(other.HasOverride, nullptr);
            NumBones = std::exchange(other.NumBones, 0);
        }

        return *this;
    }

    Skeleton::Skeleton()
        : Serializable(TypeID_Core::TID_Skeleton)
    { }

    Skeleton::Skeleton(BONE_DESC* bones, UINT32 numBones)
        : Serializable(TypeID_Core::TID_Skeleton)
        , _numBones(numBones)
        , _boneTransforms(te_newN<Transform>(numBones))
        , _invBindPoses(te_newN<Matrix4>(numBones))
        , _bonesInfo(te_newN<SkeletonBoneInfo>(numBones))
    {
        for (UINT32 i = 0; i < numBones; i++)
        {
            _boneTransforms[i] = bones[i].LocalTfrm;
            _invBindPoses[i] = bones[i].InvBindPose;
            _bonesInfo[i].Name = bones[i].Name;
            _bonesInfo[i].Parent = bones[i].Parent;
        }
    }

    Skeleton::~Skeleton()
    {
        if (_boneTransforms != nullptr)
            te_deleteN(_boneTransforms, _numBones);

        if (_invBindPoses != nullptr)
            te_deleteN(_invBindPoses, _numBones);

        if (_bonesInfo != nullptr)
            te_deleteN(_bonesInfo, _numBones);
    }

    SPtr<Skeleton> Skeleton::Create(BONE_DESC* bones, UINT32 numBones)
    {
        Skeleton* rawPtr = new (te_allocate<Skeleton>()) Skeleton(bones, numBones);
        return te_shared_ptr<Skeleton>(rawPtr);
    }

    UINT32 Skeleton::GetRootBoneIndex() const
    {
        for (UINT32 i = 0; i < _numBones; i++)
        {
            if (_bonesInfo[i].Parent == (UINT32)-1)
                return i;
        }

        return (UINT32)-1;
    }

    Transform Skeleton::CalcBoneTransform(UINT32 idx) const
    {
        if (idx >= _numBones)
            return Transform::IDENTITY;

        Transform output = _boneTransforms[idx];

        UINT32 parentIdx = _bonesInfo[idx].Parent;
        while (parentIdx != (UINT32)-1)
        {
            output.MakeWorld(_boneTransforms[parentIdx]);
            parentIdx = _bonesInfo[parentIdx].Parent;
        }

        return output;
    }

    SPtr<Skeleton> Skeleton::CreateEmpty()
    {
        Skeleton* rawPtr = new (te_allocate<Skeleton>()) Skeleton();

        SPtr<Skeleton> newSkeleton = te_shared_ptr<Skeleton>(rawPtr);
        return newSkeleton;
    }
}
