#include "Animation/TeAnimationClip.h"
#include "Resources/TeResource.h"
#include "Animation/TeSkeleton.h"
#include "Resources/TeResourceManager.h"

namespace te
{
    void AnimationCurves::AddPositionCurve(const String& name, const TAnimationCurve<Vector3>& curve)
    {
        auto iterFind = std::find_if(Position.begin(), Position.end(), [&](auto x) { return x.Name == name; });

        if (iterFind != Position.end())
            iterFind->Curve = curve;
        else
            Position.push_back({ name, AnimationCurveFlags(), curve });
    }

    void AnimationCurves::AddRotationCurve(const String& name, const TAnimationCurve<Quaternion>& curve)
    {
        auto iterFind = std::find_if(Rotation.begin(), Rotation.end(), [&](auto x) { return x.Name == name; });

        if (iterFind != Rotation.end())
            iterFind->Curve = curve;
        else
            Rotation.push_back({ name, AnimationCurveFlags(), curve });
    }

    void AnimationCurves::AddScaleCurve(const String& name, const TAnimationCurve<Vector3>& curve)
    {
        auto iterFind = std::find_if(Scale.begin(), Scale.end(), [&](auto x) { return x.Name == name; });

        if (iterFind != Scale.end())
            iterFind->Curve = curve;
        else
            Scale.push_back({ name, AnimationCurveFlags(), curve });
    }

    void AnimationCurves::AddGenericCurve(const String& name, const TAnimationCurve<float>& curve)
    {
        auto iterFind = std::find_if(Generic.begin(), Generic.end(), [&](auto x) { return x.Name == name; });

        if (iterFind != Generic.end())
            iterFind->Curve = curve;
        else
            Generic.push_back({ name, AnimationCurveFlags(), curve });
    }

    void AnimationCurves::RemovePositionCurve(const String& name)
    {
        auto iterFind = std::find_if(Position.begin(), Position.end(), [&](auto x) { return x.Name == name; });

        if (iterFind != Position.end())
            Position.erase(iterFind);
    }

    void AnimationCurves::RemoveRotationCurve(const String& name)
    {
        auto iterFind = std::find_if(Rotation.begin(), Rotation.end(), [&](auto x) { return x.Name == name; });

        if (iterFind != Rotation.end())
            Rotation.erase(iterFind);
    }

    void AnimationCurves::RemoveScaleCurve(const String& name)
    {
        auto iterFind = std::find_if(Scale.begin(), Scale.end(), [&](auto x) { return x.Name == name; });

        if (iterFind != Scale.end())
            Scale.erase(iterFind);
    }

    void AnimationCurves::RemoveGenericCurve(const String& name)
    {
        auto iterFind = std::find_if(Generic.begin(), Generic.end(), [&](auto x) { return x.Name == name; });

        if (iterFind != Generic.end())
            Generic.erase(iterFind);
    }

    void AnimationCurves::ExportJson(nlohmann::json& document) const
    {
        const auto exportFunc = [](const auto& curves, nlohmann::json& doc, const String& key)
        {
            doc = nlohmann::json::array();

            for (const auto& entry : curves)
            {
                nlohmann::json curveJson;
                entry.ExportJson(curveJson);
                doc.push_back(curveJson);
            }
        };

        exportFunc(Position, document["position"], "position");
        exportFunc(Rotation, document["rotation"], "rotation");
        exportFunc(Scale, document["scale"], "scale");
        exportFunc(Generic, document["generic"], "generic");
    }

    bool AnimationCurves::ImportJson(const nlohmann::json& document, AnimationCurves& curves)
    {
        for (const auto& curveJson : document["position"])
        {
            TNamedAnimationCurve<Vector3> curve;
            if (TNamedAnimationCurve<Vector3>::ImportJson(curveJson, curve))
            {
                curves.AddPositionCurve(curve.Name, curve.Curve);
            }
        }

        for (const auto& curveJson : document["rotation"])
        {
            TNamedAnimationCurve<Quaternion> curve;
            if (TNamedAnimationCurve<Quaternion>::ImportJson(curveJson, curve))
            {
                curves.AddRotationCurve(curve.Name, curve.Curve);
            }
        }

        for (const auto& curveJson : document["scale"])
        {
            TNamedAnimationCurve<Vector3> curve;
            if (TNamedAnimationCurve<Vector3>::ImportJson(curveJson, curve))
            {
                curves.AddScaleCurve(curve.Name, curve.Curve);
            }
        }

        for (const auto& curveJson : document["generic"])
        {
            TNamedAnimationCurve<float> curve;
            if (TNamedAnimationCurve<float>::ImportJson(curveJson, curve))
            {
                curves.AddGenericCurve(curve.Name, curve.Curve);
            }
        }

        return true;
    }

    void RootMotion::ExportJson(nlohmann::json& document) const
    {
        Position.ExportJson(document["position"]);
        Rotation.ExportJson(document["rotation"]);
    }

    bool RootMotion::ImportJson(const nlohmann::json& document, RootMotion& curves)
    {
        TAnimationCurve<Vector3>::ImportJson(document["position"], curves.Position);
        TAnimationCurve<Quaternion>::ImportJson(document["rotation"], curves.Rotation);

        return true;
    }

    AnimationClip::AnimationClip()
        : Resource(CoreType::TID_AnimationClip)
        , _curves(te_shared_ptr_new<AnimationCurves>())
        , _rootMotion(te_shared_ptr_new<RootMotion>())
        , _isAdditive(false)
        , _length(0.0f)
        , _sampleRate(0.0f)
    { }

    AnimationClip::AnimationClip(const SPtr<AnimationCurves>& curves, bool isAdditive, 
        float sampleRate, const SPtr<RootMotion>& rootMotion)
        : Resource(CoreType::TID_AnimationClip)
        , _curves(curves)
        , _rootMotion(rootMotion)
        , _isAdditive(isAdditive)
        , _length(0.0f)
        , _sampleRate(sampleRate)
    { 
        if (_curves == nullptr)
            _curves = te_shared_ptr_new<AnimationCurves>();

        if (_rootMotion == nullptr)
            _rootMotion = te_shared_ptr_new<RootMotion>();

        BuildNameMapping();
        CalculateLength();
    }

    HAnimationClip AnimationClip::Create(bool isAdditive)
    {
        return static_resource_cast<AnimationClip>(gResourceManager()._createResourceHandle(
            CreatePtr(te_shared_ptr_new<AnimationCurves>())));
    }

    HAnimationClip AnimationClip::Create(const SPtr<AnimationCurves>& curves, bool isAdditive, float sampleRate,
        const SPtr<RootMotion>& rootMotion)
    {
        return static_resource_cast<AnimationClip>(gResourceManager()._createResourceHandle(
            CreatePtr(curves, isAdditive, sampleRate, rootMotion)));
    }

    SPtr<AnimationClip> AnimationClip::CreateEmpty()
    {
        AnimationClip* rawPtr = new (te_allocate<AnimationClip>()) AnimationClip();

        SPtr<AnimationClip> newClip = te_core_ptr<AnimationClip>(rawPtr);
        newClip->SetThisPtr(newClip);

        return newClip;
    }

    SPtr<AnimationClip> AnimationClip::CreatePtr(const SPtr<AnimationCurves>& curves, bool isAdditive, float sampleRate,
        const SPtr<RootMotion>& rootMotion)
    {
        AnimationClip* rawPtr = new (te_allocate<AnimationClip>()) AnimationClip(curves, isAdditive, sampleRate, rootMotion);

        SPtr<AnimationClip> newClip = te_core_ptr<AnimationClip>(rawPtr);
        newClip->SetThisPtr(newClip);
        newClip->Initialize();

        return newClip;
    }

    void AnimationClip::SetCurves(const AnimationCurves& curves)
    {
        *_curves = curves;

        BuildNameMapping();
        CalculateLength();
    }

    bool AnimationClip::HasRootMotion() const
    {
        return _rootMotion != nullptr &&
            (_rootMotion->Position.GetNumKeyFrames() > 0 || _rootMotion->Rotation.GetNumKeyFrames() > 0);
    }

    void AnimationClip::CalculateLength()
    {
        _length = 0.0f;

        for (auto& entry : _curves->Position)
            _length = std::max(_length, entry.Curve.GetLength());

        for (auto& entry : _curves->Rotation)
            _length = std::max(_length, entry.Curve.GetLength());

        for (auto& entry : _curves->Scale)
            _length = std::max(_length, entry.Curve.GetLength());

        for (auto& entry : _curves->Generic)
            _length = std::max(_length, entry.Curve.GetLength());
    }

    void AnimationClip::BuildNameMapping()
    {
        _nameMapping.clear();

        auto registerEntries = [&](auto& curve, CurveType type)
        {
            UINT32 typeIdx = (UINT32)type;

            for (UINT32 i = 0; i < (UINT32)curve.size(); i++)
            {
                auto& entry = curve[i];

                auto iterFind = _nameMapping.find(entry.Name);
                if (iterFind == _nameMapping.end())
                {
                    UINT32* indices = _nameMapping[entry.Name].data();
                    memset(indices, -1, sizeof(UINT32) * (int)CurveType::Count);

                    indices[typeIdx] = i;
                }
                else
                {
                    _nameMapping[entry.Name][typeIdx] = i;
                }
            }
        };

        registerEntries(_curves->Position, CurveType::Position);
        registerEntries(_curves->Rotation, CurveType::Rotation);
        registerEntries(_curves->Scale, CurveType::Scale);

        // Generic and morph curves
        {
            Vector<TNamedAnimationCurve<float>>& curve = _curves->Generic;
            for (UINT32 i = 0; i < (UINT32)curve.size(); i++)
            {
                auto& entry = curve[i];
                UINT32 typeIdx = (UINT32)CurveType::Generic;

                auto iterFind = _nameMapping.find(entry.Name);
                if (iterFind == _nameMapping.end())
                {
                    UINT32* indices = _nameMapping[entry.Name].data();
                    memset(indices, -1, sizeof(UINT32) * (int)CurveType::Count);

                    indices[typeIdx] = i;
                }
                else
                {
                    _nameMapping[entry.Name][typeIdx] = i;
                }
            }
        }
    }

    void AnimationClip::Initialize()
    {
        Resource::Initialize();
        BuildNameMapping();
        CalculateLength();
    }

    void AnimationClip::GetBoneMapping(const Skeleton& skeleton, AnimationCurveMapping* mapping) const
    {
        UINT32 numBones = skeleton.GetNumBones();
        for (UINT32 i = 0; i < numBones; i++)
        {
            const SkeletonBoneInfo& boneInfo = skeleton.GetBoneInfo(i);

            GetCurveMapping(boneInfo.Name, mapping[i]);
        }
    }

    void AnimationClip::GetCurveMapping(const String& name, AnimationCurveMapping& mapping) const
    {
        auto iterFind = _nameMapping.find(name);
        if (iterFind != _nameMapping.end())
        {
            const UINT32* indices = iterFind->second.data();

            mapping.Position = indices[(UINT32)CurveType::Position];
            mapping.Rotation = indices[(UINT32)CurveType::Rotation];
            mapping.Scale = indices[(UINT32)CurveType::Scale];
        }
        else
        {
            mapping = { (UINT32)-1, (UINT32)-1, (UINT32)-1 };
        }
    }

    void AnimationClip::Serialize(StreamWriter* serializer) const
    {
        Resource::Serialize(serializer);

        nlohmann::json document;

        document["isAdditive"] = _isAdditive;
        document["sampleRate"] = _sampleRate;

        for (const auto& event : _events)
        {
            nlohmann::json eventJson;
            eventJson["name"] = event.Name;
            eventJson["time"] = event.Time;
            document["events"].push_back(eventJson);
        }

        document["curves"] = nlohmann::json::object();
        _curves->ExportJson(document["curves"]);

        document["rootMotion"] = nlohmann::json::object();
        _rootMotion->ExportJson(document["rootMotion"]);

        String dump = document.dump();
        serializer->WriteString(dump);
    }

    bool AnimationClip::Deserialize(StreamReader* deserializer, AnimationClip* object)
    {
        if (!object)
            return false;

        Resource::Deserialize(deserializer, object);

        String dump;
        deserializer->ReadString(dump);
        nlohmann::json document = nlohmann::json::parse(dump);

        object->_isAdditive = document["isAdditive"].get<bool>();
        object->_sampleRate = document["sampleRate"].get<float>();

        for (const auto& eventJson : document["events"])
        {
            AnimationEvent event;
            event.Name = eventJson["name"].get<String>();
            event.Time = eventJson["time"].get<float>();
            object->_events.push_back(event);
        }

        AnimationCurves::ImportJson(document["curves"], *object->_curves);
        RootMotion::ImportJson(document["rootMotion"], *object->_rootMotion);

        object->Initialize();

        return true;
    }
}
