
#pragma once

#include "MeshRenderer.h"
#include "Runtime/Function/Framework/Component/Animation/animator.h"

namespace LitchiRuntime
{
	/**
	 * @brief SkinnedMeshRenderer Component
	 * @note The Skinned Mesh Renderer component renders a deformable mesh.
	 *		 Deformable meshes include skinned meshes (meshes that have bones and bind poses),
	 *		 meshes that have blend shapes, and meshes that run cloth simulation.
	*/
	class SkinnedMeshRenderer : public MeshRenderer
	{
	public:

		/**
		 * @brief Default Constructor
		*/
		SkinnedMeshRenderer();

		/**
		 * @brief Default Destructor
		*/
		~SkinnedMeshRenderer() override;

		/**
		 * @brief Get Bone cbuffer
		 * @return 
		*/
		std::shared_ptr<RHI_ConstantBuffer> GetBoneConstantBuffer() { return m_boneConstantBuffer; }

		/**
		 * @brief Get MeshRendererType
		 * @return
		*/
		MeshRendererType GetMeshRendererType() const override { return MeshRendererType::SkinMeshRenderer; }


		/**
		 * @brief Set Default PBR Material
		 * @return
		*/
		void SetDefaultMaterial() override;

		/**
		 * @brief Call before object resource change
		*/
		void PostResourceModify() override;

		/**
		 * @brief Call before object resource loaded
		 * when instantiate prefab, add component, resource loaded etc
		 * after call resource load completed
		*/
		void PostResourceLoaded() override;

	public:

		/**
		 * @brief Called when the scene start right before OnStart
		 * It allows you to apply prioritized game logic on scene start
		*/
		void OnAwake() override;

		/**
		 * @brief Called when the components gets enabled (owner SetActive set to true) and after OnAwake() on scene starts
		*/
		void OnUpdate() override;

	private:

		/**
		 * @brief Create Bone cbuffer resource
		*/
		void CreateBoneBuffer();

		/**
		 * @brief Create default Bone buffer
		*/
		void CreateDefaultBoneBuffer();

		/**
		 * @brief Calculate Bone Default Final Transform, this is T-Pose Bone Transform
		 * @param boneHierarchy 
		 * @param nodelDefaultTransforms 
		*/
		void CalcDefaultFinalTransform(std::vector<int>& boneHierarchy, std::vector<Matrix>& nodelDefaultTransforms);

		/**
		 * @brief Calculate Final Transform in current frame
		 * @param timePos Time position
		 * @param clip AnimationClip
		 * @param boneHierarchy Bone Hierarchy indices
		 * @param boneOffsets T-Pose transform, make vertex transform to bone space
		*/
		void CalcFinalTransform(float timePos, AnimationClip& clip, std::vector<int>& boneHierarchy, std::vector<Matrix>& boneOffsets);

		/**
		 * @brief Find Animator, if current object not have, find parent util not exist
		*/
		Animator* FindAnimatorInHierarchy();

		/**
		 * @brief is dirty
		*/
		bool m_isDirty = true;

		/**
		 * @brief is First Update called
		*/
		bool m_isFirstTick = true;

		/**
		 * @brief Bone Array Cb 
		*/
		Cb_Bone_Arr m_boneArr;

		/**
		 * @brief bone cbuffer shared_pointer
		*/
		std::shared_ptr<RHI_ConstantBuffer> m_boneConstantBuffer;

		RTTR_ENABLE(MeshRenderer)
	};

}
