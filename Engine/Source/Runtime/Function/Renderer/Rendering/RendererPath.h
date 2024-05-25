
#pragma once

#include "Renderer_Definitions.h"
#include "Runtime/Function/Framework/Component/Light/Light.h"
#include "Runtime/Function/Framework/Component/UI/UICanvas.h"
#include "Runtime/Function/Renderer/RenderCamera.h"
#include "Runtime/Function/Scene/SceneManager.h"


namespace LitchiRuntime
{

	enum RendererPathType
	{
		RendererPathType_Invalid,
		RendererPathType_SceneView,
		RendererPathType_GameView,
		RendererPathType_AssetView,
		RendererPathType_Custom,
		RendererPathType_Count
	};

	enum SelectedResourceType
	{
		SelectedResourceType_None,
		SelectedResourceType_Material,
		SelectedResourceType_Mesh,
		SelectedResourceType_Texture2D,
	};

	// rendererPath, completed once pass, render to texture
	class RendererPath
	{
	public:
		RendererPath(RendererPathType rendererPathType);
		~RendererPath();
	public:

		void Update();

		// Basic
		RendererPathType GetRendererPathType() { return m_rendererPathType; }
		void SetActive(bool isActive) { m_active = isActive; }
		bool GetActive()const { return m_active; }

		// Scene
		void SetScene(Scene* scene);
		Scene* GetRenderScene() { return m_renderScene; }
		std::unordered_map<Renderer_Entity, std::vector<GameObject*>>& GetRenderables() { return m_renderables; }
		bool HasTransparentMesh();
		int64_t GetMeshIndexTransparent() const {return m_meshIndexTransparent; }
		int64_t GetMeshIndexNonInstancedOpaque()const { return m_meshIndexNonInstancedOpaque; }
		int64_t GetMeshIndexNonInstancedTransparent()const { return m_meshIndexNonInstancedTransparent; }

		// Camera
		void SetRenderCamera(RenderCamera* camera);
		RenderCamera* GetRenderCamera(){return m_renderCamera;}

		// RT
		void SetRenderTarget(float width, float height);
		std::shared_ptr<RHI_Texture> GetColorRenderTarget() { return m_colorRenderTarget; }
		std::shared_ptr<RHI_Texture> GetDepthRenderTarget() { return m_depthRenderTarget; }
		std::shared_ptr<RHI_StructuredBuffer> GetLightBuffer() { return m_light_structure_buffer; }

		UICanvas* GetCanvas();
		Light* GetMainLight() const { return m_mainLight; }
		size_t GetLightCount() const;

		//-- SelectedAsset  --//
		void UpdateSelectedAssetViewResource(Material* material, Mesh* mesh, RHI_Texture2D* texture_2d);
		Material* GetSelectedMaterial() const { return m_selectedMaterial; }
		Mesh* GetSelectedMesh() const { return m_selectedMesh; }
		RHI_Texture2D* GetSelectedTexture2D() const { return m_selectedTexture2D; }
		SelectedResourceType GetSelectedResourceType() const { return m_selectedResType; }

		// Light & Shadow
		const Matrix& GetLightViewMatrix(uint32_t index = 0) const;
		const Matrix& GetLightProjectionMatrix(uint32_t index = 0) const;
		RHI_Texture* GetShadowDepthTexture() const { return m_shadow_map.texture_depth.get(); }
		RHI_Texture* GetShadowColorTexture() const { return m_shadow_map.texture_color.get(); }
		uint32_t GetShadowArraySize() const;
		bool IsInLightViewFrustum(MeshFilter* renderable, uint32_t index) const;

		// Bone
		std::shared_ptr<RHI_ConstantBuffer>  GetSelectedMeshBoneConstantBuffer() { return m_selectedMesh_bone_constant_buffer; }

	private:

		void UpdateSceneObject();
		void UpdateLight();

		void CreateColorRenderTarget();
		void CreateDepthRenderTarget();
		void CreateLightBuffer();

		std::string GetRenderPathName() const;
		bool CheckIsBuildInRendererCamera();

		void UpdateLightBuffer();
		void UpdateDefaultLightBuffer();
		bool CheckShadowMapNeedRecreate();
		void CreateShadowMap();
		void ComputeLightViewMatrix();
		void ComputeLightProjectionMatrix();


        float GetSquaredDistance(const GameObject* entity);
        void FrustumCulling(std::vector<GameObject*>& renderables);
        void Sort(std::vector<GameObject*>& renderables);
        void FrustumCullAndSort(std::vector<GameObject*>& renderables);
	private:

		//-- Common --//
		bool m_active{ false };
		RendererPathType m_rendererPathType = RendererPathType_Invalid;
		RenderCamera* m_renderCamera = nullptr;
		// scene 
		Scene* m_renderScene = nullptr;
		// renderables
		std::unordered_map<Renderer_Entity, std::vector<GameObject*>> m_renderables;
		std::unordered_map<uint64_t, float> distances_squared;
		int64_t m_meshIndexTransparent = 0;
		int64_t m_meshIndexNonInstancedOpaque = 0;
		int64_t m_meshIndexNonInstancedTransparent = 0;

		// UI provide Camera
		RenderCamera* m_renderCamera4UI;
		float m_width, m_height;
		// rt
		std::shared_ptr<RHI_Texture> m_depthRenderTarget = nullptr;
		std::shared_ptr<RHI_Texture> m_colorRenderTarget = nullptr;

		//-- Light --//
		// Light buffer
		std::shared_ptr<RHI_StructuredBuffer> m_light_structure_buffer;
		// one camera to light
		Light* m_mainLight = nullptr;
		std::array<Matrix, 6> m_matrix_view;
		std::array<Matrix, 6> m_matrix_projection;
		std::array<Frustum, 6> m_frustums;
		// shadow
		ShadowMap m_shadow_map; // correct is <tuple<light,camera>,shadowMap>
		bool m_last_shadows_enabled = false;
		bool m_last_shadows_transparent_enabled = false;

		//-- Selected Asset --//
		// Select Material
		Material* m_selectedMaterial = nullptr;
		RHI_Texture2D* m_selectedTexture2D = nullptr;
		Mesh* m_selectedMesh = nullptr;
		SelectedResourceType m_selectedResType{SelectedResourceType_None};

		// Bone
		std::shared_ptr<RHI_ConstantBuffer> m_selectedMesh_bone_constant_buffer;

	};



	class MultiCameraRendererPath:public RendererPath
	{
		
	};
}
