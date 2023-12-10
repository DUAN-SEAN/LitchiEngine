//#pragma once
//
//#include <string>
//#include <unordered_map>
//#include <vector>
//
//#include "Runtime/Core/Math/Matrix.h"
//#include "Runtime/Core/Math/Quaternion.h"
//#include "Runtime/Core/Math/Vector3.h"
//
//struct VectorKey {
//	float timePos;
//	LitchiRuntime::Vector3 value;
//};
//
//struct QuatKey {
//	float timePos;
//	LitchiRuntime::Quaternion value;
//};
//
//class BoneAnimation {
//public:
//	float GetStartTime()const;
//	float GetEndTime()const;
//	void Interpolate(float t, LitchiRuntime::Matrix& M);
//
//	std::vector<VectorKey> translation;
//	std::vector<VectorKey> scale;
//	std::vector<QuatKey> rotationQuat;
//
//	LitchiRuntime::Matrix defaultTransform;
//
//private:
//	LitchiRuntime::Vector3 LerpKeys(float t, const std::vector<VectorKey>& keys);
//	LitchiRuntime::Quaternion LerpKeys(float t, const std::vector<QuatKey>& keys);
//};
//
//class AnimationClip {
//public:
//	float GetClipStartTime()const;
//	float GetClipEndTime()const;
//	void Interpolate(float t, std::vector<LitchiRuntime::Matrix>& boneTransform);
//
//	std::vector<BoneAnimation> boneAnimations;
//};
//
//class SkinnedData {
//public:
//	uint32_t GetBoneCount()const { return boneHierarchy.size(); }
//	float GetClipStartTime(const std::string& clipName)const;
//	float GetClipEndTime(const std::string& clipName)const;
//	void Set(std::vector<int>& boneHierarchy,
//		std::vector<LitchiRuntime::Matrix>& boneOffsets,
//		std::unordered_map<std::string, AnimationClip>& animations);
//	void GetFinalTransform(const std::string& clipName, float timePos, std::vector<LitchiRuntime::Matrix>& finalTransforms);
//
//private:
//	std::vector<int> boneHierarchy;
//	std::vector<LitchiRuntime::Matrix> boneOffsets;
//	std::unordered_map<std::string, AnimationClip> animations;
//};
//
//// Temp 
//struct SkinnedModelInstance {
//	SkinnedData skinnedInfo;
//	std::vector<LitchiRuntime::Matrix> finalTransforms;
//	std::string clipName;
//	float timePos = 0.0f;
//
//	uint32_t skinnedCBIndex;
//
//	void UpdateSkinnedAnimation(float deltaTime) {
//		timePos += deltaTime;
//
//		//Loop
//		if (timePos > skinnedInfo.GetClipEndTime(clipName))
//			timePos = 0.0f;
//
//		skinnedInfo.GetFinalTransform(clipName, timePos, finalTransforms);
//	}
//};
