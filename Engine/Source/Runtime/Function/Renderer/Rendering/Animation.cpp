

//= INCLUDES =========
#include "Runtime/Core/pch.h"
#include "Animation.h"
//====================

//= NAMESPACES =====
using namespace std;
//==================

namespace LitchiRuntime
{
	float BoneAnimation::GetStartTime()const {
		float t0 = 0.0f;
		float t1 = 0.0f;
		float t2 = 0.0f;
		if (translation.size() != 0) t0 = translation.front().timePos;
		if (scale.size() != 0) t1 = scale.front().timePos;
		if (rotationQuat.size() != 0) t2 = rotationQuat.front().timePos;

		float timePos = t0 < t1 ? t0 : t1;
		timePos = timePos < t2 ? timePos : t2;
		return timePos;
	}

	float BoneAnimation::GetEndTime()const {
		float t0 = 0.0f;
		float t1 = 0.0f;
		float t2 = 0.0f;
		if (translation.size() != 0) t0 = translation.back().timePos;
		if (scale.size() != 0) t1 = scale.back().timePos;
		if (rotationQuat.size() != 0) t2 = rotationQuat.back().timePos;

		float timePos = t0 > t1 ? t0 : t1;
		timePos = timePos > t2 ? timePos : t2;
		return timePos;
	}

	LitchiRuntime::Vector3 BoneAnimation::LerpKeys(float t, const std::vector<VectorKey>& keys) {
		Vector3 res = Vector3(0.0f);
		if (t <= keys.front().timePos) res = keys.front().value;
		else if (t >= keys.back().timePos) res = keys.back().value;
		else {
			for (size_t i = 0; i < keys.size() - 1; i++) {
				if (t >= keys[i].timePos && t <= keys[i + 1].timePos) {
					float lerpPercent = (t - keys[i].timePos) / (keys[i + 1].timePos - keys[i].timePos);
					res = keys[i].value * lerpPercent + keys[i + 1].value * (1 - lerpPercent);
					break;
				}
			}
		}
		return res;
	}

	Quaternion BoneAnimation::LerpKeys(float t, const std::vector<QuatKey>& keys) {
		Quaternion res = Quaternion();
		if (t <= keys.front().timePos) res = keys.front().value;
		else if (t >= keys.back().timePos) res = keys.back().value;
		else {
			for (size_t i = 0; i < keys.size() - 1; i++) {
				if (t >= keys[i].timePos && t <= keys[i + 1].timePos) {
					float lerpPercent = (t - keys[i].timePos) / (keys[i + 1].timePos - keys[i].timePos);
					res = Quaternion::Lerp(keys[i].value, keys[i + 1].value, lerpPercent);
					break;
				}
			}
		}
		return res;
	}

	void BoneAnimation::Interpolate(float t, Matrix& M) {
		if (translation.size() == 0 && scale.size() == 0 && rotationQuat.size() == 0) {
			M = defaultTransform;
			return;
		}

		Vector3 T = Vector3(0.0f);
		Vector3 S = Vector3(0.0f);
		Quaternion R = Quaternion();

		if (translation.size() != 0) T = LerpKeys(t, translation);
		if (scale.size() != 0) S = LerpKeys(t, scale);
		if (rotationQuat.size() != 0) R = LerpKeys(t, rotationQuat);

		M = Matrix(T, R, S);
	}

	float AnimationClip::GetClipStartTime()const {
		float t = FLT_MAX;
		for (uint32_t i = 0; i < boneAnimations.size(); i++)
			t = t < boneAnimations[i].GetStartTime() ? t : boneAnimations[i].GetStartTime();
		return t;
	}

	float AnimationClip::GetClipEndTime()const {
		float t = 0.0f;
		for (uint32_t i = 0; i < boneAnimations.size(); i++)
			t = t > boneAnimations[i].GetEndTime() ? t : boneAnimations[i].GetEndTime();
		return t;
	}

	void AnimationClip::Interpolate(float t, std::vector<Matrix>& boneTransform) {
		for (uint32_t i = 0; i < boneAnimations.size(); i++)
			boneAnimations[i].Interpolate(t, boneTransform[i]);
	}

    Animation::Animation(): IResource(ResourceType::Animation)
    {

    }

    bool Animation::LoadFromFile(const string& filePath)
    {
        return true;
    }

    bool Animation::SaveToFile(const string& filePath)
    {
        return true;
    }
}
