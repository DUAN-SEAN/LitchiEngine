
#pragma once

#include <vector>
#include <unordered_map>
#include <string>

#include "Mesh.h"
#include "SkinnedData.h"

#include "Runtime/Core/Log/debug.h"

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
			glm::mat4x4 boneOffset;
			glm::mat4x4 defaultOffset;
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

	private:
		Model(const std::string& p_path);
		~Model();

		void ComputeBoundingSphere();

	public:
		const std::string path;

		std::vector<Mesh*> m_meshes;
		std::vector<std::string> m_materialNames;

		BoundingSphere m_boundingSphere;


		//Bone/Animation Information
		std::vector<BoneInfo> boneInfo;
		std::unordered_map<std::string, uint32_t> boneMapping;
		std::unordered_map<std::string, AnimationClip> animations;
	private:
	};
}
