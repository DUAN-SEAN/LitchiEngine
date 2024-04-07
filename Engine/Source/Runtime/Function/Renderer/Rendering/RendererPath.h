
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

		RendererPathType GetRendererPathType() { return m_rendererPathType; }

		bool GetActive()const { return m_active; }
		void SetActive(bool isActive) { m_active = isActive; }

		RenderCamera* GetRenderCamera(){return m_renderCamera;}
		void UpdateRenderCamera(RenderCamera* camera);

		std::shared_ptr<RHI_Texture> GetColorRenderTarget() { return m_colorRenderTarget; }
		std::shared_ptr<RHI_Texture> GetDepthRenderTarget() { return m_depthRenderTarget; }

		void UpdateRenderTarget(float width, float height);
		void UpdateRenderableGameObject();
		void UpdateLightShadow();

		void UpdateScene(Scene* scene);
		Scene* GetRenderScene() { return m_renderScene; }
		const std::unordered_map<Renderer_Entity, std::vector<GameObject*>>& GetRenderables() { return m_renderables; }
		UICanvas* GetCanvas();

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
		std::shared_ptr<RHI_ConstantBuffer>  GetSelectedMeshBoneConstantBuffer() { return m_selectedMesh_bone_constant_buffer; }
		uint32_t GetShadowArraySize() const;
		bool IsInLightViewFrustum(MeshFilter* renderable, uint32_t index) const;

	private:

		void CreateColorRenderTarget();
		void CreateDepthRenderTarget();
		std::string GetRenderPathName() const;
		bool CheckIsBuildInRendererCamera();

		bool CheckShadowMapNeedRecreate();
		void CreateShadowMap();
		void ComputeLightViewMatrix();
		void ComputeLightProjectionMatrix();

	private:

		bool m_active{ false };

		RendererPathType m_rendererPathType = RendererPathType_Invalid;
		RenderCamera* m_renderCamera = nullptr;

		// scene 
		Scene* m_renderScene = nullptr;
		std::unordered_map<Renderer_Entity, std::vector<GameObject*>> m_renderables;

		// UI provide Camera
		RenderCamera* m_renderCamera4UI;

		// rt
		float m_width;
		float m_height;
		std::shared_ptr<RHI_Texture> m_depthRenderTarget = nullptr;
		std::shared_ptr<RHI_Texture> m_colorRenderTarget = nullptr;

		// one camera to light

		// Light
		Light* m_mainLight = nullptr;
		std::array<Matrix, 6> m_matrix_view;
		std::array<Matrix, 6> m_matrix_projection;
		std::array<Frustum, 6> m_frustums;

		// shadow
		ShadowMap m_shadow_map; // correct is <tuple<light,camera>,shadowMap>
		bool m_last_shadows_enabled = false;
		bool m_last_shadows_transparent_enabled = false;

		// Select Material
		Material* m_selectedMaterial = nullptr;
		RHI_Texture2D* m_selectedTexture2D = nullptr;
		Mesh* m_selectedMesh = nullptr;
		std::shared_ptr<RHI_ConstantBuffer> m_selectedMesh_bone_constant_buffer;
		SelectedResourceType m_selectedResType{SelectedResourceType_None};

	};

}
