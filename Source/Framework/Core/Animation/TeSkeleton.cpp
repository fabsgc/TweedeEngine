#include "TeSkeleton.h"

#include "Animation/TeAnimationClip.h"

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

    LocalSkeletonPose::LocalSkeletonPose(LocalSkeletonPose&& other) noexcept
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

    LocalSkeletonPose& LocalSkeletonPose::operator=(LocalSkeletonPose&& other) noexcept
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

    void Skeleton::GetPose(Matrix4* pose, LocalSkeletonPose& localPose, const SkeletonMask& mask,
        const AnimationClip& clip, float time, bool loop)
    {
        Vector<AnimationCurveMapping> boneToCurveMapping(_numBones);

        AnimationState state;
        state.Curves = clip.GetCurves();
        state.Length = clip.GetLength();
        state.BoneToCurveMapping = boneToCurveMapping.data();
        state.Loop = loop;
        state.Weight = 1.0f;
        state.Time = time;
        state.Disabled = false;

        AnimationStateLayer layer;
        layer.Index = 0;
        layer.States = &state;
        layer.NumStates = 1;

        clip.GetBoneMapping(*this, state.BoneToCurveMapping);

        GetPose(pose, localPose, mask, &layer, 1);
    }

    void Skeleton::GetPose(Matrix4* pose, LocalSkeletonPose& localPose, const SkeletonMask& mask,
        const AnimationStateLayer* layers, UINT32 numLayers)
    {
        assert(localPose.NumBones == _numBones);

        for (UINT32 i = 0; i < _numBones; i++)
        {
            localPose.Positions[i] = Vector3::ZERO;
            localPose.Rotations[i] = Quaternion::ZERO;
            localPose.Scales[i] = Vector3::ONE;
        }

        bool* hasAnimCurve = te_newN<bool>(_numBones);
        te_zero_out(hasAnimCurve, _numBones);

        for (UINT32 i = 0; i < numLayers; i++)
        {
            const AnimationStateLayer& layer = layers[i];
            float invLayerWeight = 1.0f;

            for (UINT32 j = 0; j < layer.NumStates; j++)
            {
                const AnimationState& state = layer.States[j];
                if (state.Disabled)
                    continue;

                float normWeight = state.Weight * invLayerWeight;

                if (Math::ApproxEquals(normWeight, 0.0f))
                    continue;

                for (UINT32 k = 0; k < _numBones; k++)
                {
                    if (!mask.IsEnabled(k))
                        continue;

                    const AnimationCurveMapping& mapping = state.BoneToCurveMapping[k];
                    UINT32 curveIdx = mapping.Position;
                    if (curveIdx != (UINT32)-1)
                    {
                        const TAnimationCurve<Vector3>& curve = state.Curves->Position[curveIdx].Curve;
                        localPose.Positions[k] += curve.Evaluate(state.Time, false) * normWeight;

                        localPose.HasOverride[k] = false;
                        hasAnimCurve[k] = true;
                    }

                    curveIdx = mapping.Scale;
                    if (curveIdx != (UINT32)-1)
                    {
                        const TAnimationCurve<Vector3>& curve = state.Curves->Scale[curveIdx].Curve;
                        localPose.Scales[k] *= curve.Evaluate(state.Time, false) * normWeight;

                        localPose.HasOverride[k] = false;
                        hasAnimCurve[k] = true;
                    }

                    curveIdx = mapping.Rotation;
                    if (curveIdx != (UINT32)-1)
                    {
                        const TAnimationCurve<Quaternion>& curve = state.Curves->Rotation[curveIdx].Curve;
                        Quaternion value = curve.Evaluate(state.Time, false) * normWeight;

                        if (value.Dot(localPose.Rotations[k]) < 0.0f)
                            value = -value;

                        localPose.Rotations[k] += value;
                        localPose.HasOverride[k] = false;
                        hasAnimCurve[k] = true;
                    }
                }
            }
        }

        // Apply default local tranform to non-animated bones (so that any potential child bones are transformed properly)
        for (UINT32 i = 0; i < _numBones; i++)
        {
            if (hasAnimCurve[i])
                continue;

            localPose.Positions[i] = _boneTransforms[i].GetPosition();
            localPose.Rotations[i] = _boneTransforms[i].GetRotation();
            localPose.Scales[i] = _boneTransforms[i].GetScale();
        }

        // Calculate local pose matrices
        UINT32 isGlobalBytes = sizeof(bool) * _numBones;
        bool* isGlobal = (bool*)te_allocate(isGlobalBytes);
        memset(isGlobal, 0, isGlobalBytes);

        for (UINT32 i = 0; i < _numBones; i++)
        {
            bool isAssigned = localPose.Rotations[i].w != 0.0f;
            if (!isAssigned)
                localPose.Rotations[i] = Quaternion::IDENTITY;
            else
                localPose.Rotations[i].Normalize();

            if (localPose.HasOverride[i])
            {
                isGlobal[i] = true;
                continue;
            }

            pose[i] = Matrix4::TRS(localPose.Positions[i], localPose.Rotations[i], localPose.Scales[i]);
        }

        // Calculate global poses
        // Note: For a possible performance improvement consider sorting bones in such order so that parents (and overrides)
        // always come before children, we no isGlobal check is needed.
        std::function<void(UINT32)> calcGlobal = [&](UINT32 boneIdx)
        {
            UINT32 parentBoneIdx = _bonesInfo[boneIdx].Parent;
            if (parentBoneIdx == (UINT32)-1)
            {
                isGlobal[boneIdx] = true;
                return;
            }

            if (!isGlobal[parentBoneIdx])
                calcGlobal(parentBoneIdx);

            pose[boneIdx] = pose[parentBoneIdx] * pose[boneIdx];
            isGlobal[boneIdx] = true;
        };

        for (UINT32 i = 0; i < _numBones; i++)
        {
            if (!isGlobal[i])
                calcGlobal(i);
        }

        for (UINT32 i = 0; i < _numBones; i++)
            pose[i] = pose[i] * _invBindPoses[i];

        te_delete(isGlobal);
        te_deleteN<bool>(hasAnimCurve, _numBones);
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

    Transform Skeleton::ComputeBoneTransform(UINT32 idx) const
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
