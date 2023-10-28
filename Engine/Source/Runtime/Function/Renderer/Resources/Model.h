
#pragma once

#include <vector>
#include <unordered_map>
#include <string>

//#include "Mesh.h"
#include "SkinnedData.h"

#include "Runtime/Core/Log/debug.h"
#include "Runtime/Function/Renderer/Geometry/BoundingSphere.h"
#include "Runtime/Function/Renderer/Rendering/Mesh.h"

#define NUM_BONES_PER_VERTEX 4

namespace LitchiRuntime
{
	namespace Loaders { class ModelLoader; }

	/**
	* A model is a combinaison of meshes
	*/
	class Model
	{
		friend class Loaders::ModelLoader;
	public:
		struct BoneData {
			uint32_t boneIndex[NUM_BONES_PER_VERTEX];
			float weights[NUM_BONES_PER_VERTEX];
			void Add(uint32_t boneID, float weight) {
				for (size_t i = 0; i < NUM_BONES_PER_VERTEX; i++) {
					if (weights[i] == 0.0f) {
						boneIndex[i] = boneID;
						weights[i] = weight;
						return;
					}
				}
				//insert error program
				DEBUG_LOG_ERROR("bone index out of size");
			}
		};
		struct BoneInfo {
			bool isSkinned = false;
			Matrix boneOffset;// 骨骼空间到模型空间的转变矩阵 逆Bind Pose矩阵
			Matrix defaultOffset;// 到父骨骼的变换矩阵
			int parentIndex;
		};

	public:
		/**
		* Returns the meshes
		*/
		const std::vector<Mesh*>& GetMeshes() const;

		/**
		* Returns the material names
		*/
		const std::vector<std::string>& GetMaterialNames() const;

		/**
		* Returns the bounding sphere of the model
		*/
		const LitchiRuntime::BoundingSphere& GetBoundingSphere() const;

		void GetBoneMapping(std::unordered_map<std::string, uint32_t>& boneMapping) {
			boneMapping = this->boneMap;
		}
		void GetBoneOffsets(std::vector<Matrix>& boneOffsets) {
			for (size_t i = 0; i < boneInfoArr.size(); i++)
				boneOffsets.push_back(boneInfoArr[i].boneOffset);
		}
		void GetNodeOffsets(std::vector<Matrix>& nodeOffsets) {
			for (size_t i = 0; i < boneInfoArr.size(); i++)
				nodeOffsets.push_back(boneInfoArr[i].defaultOffset);
		}
		void GetBoneHierarchy(std::vector<int>& boneHierarchy) {
			for (size_t i = 0; i < boneInfoArr.size(); i++)
				boneHierarchy.push_back(boneInfoArr[i].parentIndex);
		}
		void GetAnimations(std::unordered_map<std::string, AnimationClip>& animations) {
			animations.insert(this->animationClipMap.begin(), this->animationClipMap.end());
		}

	private:
		Model(const std::string& p_path);
		~Model();

		void ComputeBoundingSphere();

	public:
		const std::string path;

		std::vector<Mesh*> mesheArr;
		std::vector<std::string> materialNameArr;

		BoundingSphere boundingSphere;


		//Bone/Animation Information
		std::vector<BoneInfo> boneInfoArr;
		std::unordered_map<std::string, uint32_t> boneMap;
		std::unordered_map<std::string, AnimationClip> animationClipMap;
	private:
	};
}
