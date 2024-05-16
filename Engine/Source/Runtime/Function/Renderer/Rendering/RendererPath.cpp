#include "RendererPath.h"

#include <memory>
#include <memory>
#include <easy/profiler.h>
#include <easy/details/profiler_colors.h>

#include "Runtime/Function/Framework/Component/Camera/camera.h"
#include "Runtime/Function/Framework/Component/Light/Light.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshRenderer.h"
#include "Runtime/Function/Framework/Component/Renderer/SkinnedMeshRenderer.h"
#include "Runtime/Function/Framework/Component/UI/UICanvas.h"
#include "Runtime/Function/Framework/Component/UI/UIImage.h"
#include "Runtime/Function/Framework/Component/UI/UIText.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Renderer/RHI/RHI_ConstantBuffer.h"
#include "Runtime/Function/Renderer/RHI/RHI_StructuredBuffer.h"
#include "Runtime/Function/Renderer/RHI/RHI_Texture.h"
#include "Runtime/Function/Renderer/RHI/RHI_Texture2D.h"
#include "Runtime/Function/Renderer/RHI/RHI_Texture2DArray.h"
#include "Runtime/Function/Renderer/RHI/RHI_TextureCube.h"

namespace LitchiRuntime
{
	namespace
	{
		float orthographic_depth = 4096.0f; // depth of all cascades
		float orthographic_extent_near = 12.0f;
		float orthographic_extent_far = 64.0f;
	}

	RendererPath::RendererPath(RendererPathType rendererPathType)
	{
		m_rendererPathType = rendererPathType;

		// ³õÊ¼»¯Cameraf
		if (CheckIsBuildInRendererCamera())
		{
			auto renderCamera4SceneView = new RenderCamera();
			renderCamera4SceneView->Initialize();
			m_renderCamera = renderCamera4SceneView;

			m_width = m_renderCamera->GetViewport().width;
			m_height = m_renderCamera->GetViewport().height;

			m_renderables.clear();

			CreateColorRenderTarget();
			CreateDepthRenderTarget();
		}

		CreateLightBuffer();
	}

	RendererPath::~RendererPath()
	{
		if (m_renderCamera && CheckIsBuildInRendererCamera())
		{
			delete m_renderCamera;
			m_renderCamera = nullptr;
		}

		m_depthRenderTarget = nullptr;
		m_colorRenderTarget = nullptr;
		m_light_structure_buffer = nullptr;

		if (m_selectedMesh_bone_constant_buffer)
		{
			m_selectedMesh_bone_constant_buffer.reset();
		}
	}

	void RendererPath::SetScene(Scene* scene)
	{
		m_renderScene = scene;
		m_mainLight = nullptr;
	}

	void RendererPath::SetRenderCamera(RenderCamera* camera)
	{
		if (CheckIsBuildInRendererCamera())
		{
			return;
		}

		if (camera == nullptr || m_renderCamera == camera)
		{
			return;
		}

		m_renderCamera = camera;
		m_width = m_renderCamera->GetViewport().width;
		m_height = m_renderCamera->GetViewport().height;

		m_renderables.clear();

		CreateColorRenderTarget();
		CreateDepthRenderTarget();
	}


	void RendererPath::SetRenderTarget(float width, float height)
	{
		m_width = width;
		m_height = height;

		CreateColorRenderTarget();
	}

	UICanvas* RendererPath::GetCanvas()
	{
		if (this->m_renderables.find(Renderer_Entity::Canvas) != m_renderables.end())
		{
			auto canvass = this->m_renderables.at(Renderer_Entity::Canvas);
			if (canvass.size() > 0)
			{
				auto canvas = canvass[0]->GetComponent<UICanvas>();
				return canvas;
			}
		}

		return nullptr;
	}

	size_t RendererPath::GetLightCount() const
	{
		if (m_rendererPathType == RendererPathType_AssetView)
		{
			return 1;
		}

		if (this->m_renderables.find(Renderer_Entity::Light) != m_renderables.end())
		{
			return this->m_renderables.at(Renderer_Entity::Light).size();
		}
		return 0;
	}

	void RendererPath::UpdateSelectedAssetViewResource(Material* material, Mesh* mesh, RHI_Texture2D* texture_2d)
	{
		// only one select
		m_selectedMaterial = material;
		m_selectedMesh = mesh;
		m_selectedTexture2D = texture_2d;

		if (m_selectedMaterial != nullptr)
		{
			m_selectedResType = SelectedResourceType_Material;
			m_selectedMesh_bone_constant_buffer.reset();
			m_selectedMesh_bone_constant_buffer = nullptr;
		}
		else if (m_selectedMesh != nullptr)
		{
			m_selectedResType = SelectedResourceType_Mesh;

			if (m_selectedMesh_bone_constant_buffer == nullptr)
			{
				m_selectedMesh_bone_constant_buffer = std::make_unique<RHI_ConstantBuffer>();
				m_selectedMesh_bone_constant_buffer->Create<Cb_Bone_Arr>(1);
			}

			// get bone data
			std::vector<int> boneHierarchy;
			std::vector<Matrix> defaultTransform;
			m_selectedMesh->GetBoneHierarchy(boneHierarchy);
			m_selectedMesh->GetNodeOffsets(defaultTransform);

			Cb_Bone_Arr m_bone_arr;
			uint32_t numBones = defaultTransform.size();
			// 
			for (uint32_t i = 0; i < numBones; i++) {
				m_bone_arr.boneArr[i] = defaultTransform[i];
			}
			// update cbuffer
			m_selectedMesh_bone_constant_buffer->UpdateWithReset(&m_bone_arr);
		}
		else if (m_selectedTexture2D != nullptr)
		{
			m_selectedResType = SelectedResourceType_Texture2D;
			m_selectedMesh_bone_constant_buffer.reset();
			m_selectedMesh_bone_constant_buffer = nullptr;
		}
	}

	void RendererPath::CreateColorRenderTarget()
	{
		// typical flags
		uint32_t flags_rt = RHI_Texture_Uav | RHI_Texture_Srv | RHI_Texture_Rtv;

		std::string rtName = GetRenderPathName() + std::string("_frame_output");
		m_colorRenderTarget = std::make_shared<RHI_Texture2D>(m_width, m_height, 1, RHI_Format::R16G16B16A16_Float, flags_rt | RHI_Texture_ClearBlit, "frame_output");

	}

	void RendererPath::CreateDepthRenderTarget()
	{
		uint32_t flags_depth_buffer = RHI_Texture_Rtv | RHI_Texture_Srv;
		std::string rtName = GetRenderPathName() + std::string("_frame_depth");

		m_depthRenderTarget = std::make_shared<RHI_Texture2D>(m_width, m_height, 1, RHI_Format::D32_Float, flags_depth_buffer, rtName.c_str());
	}

	void RendererPath::CreateLightBuffer()
	{
		uint32_t stride = static_cast<uint32_t>(sizeof(Sb_Light)) * rhi_max_array_size_lights;
		m_light_structure_buffer = std::make_shared<RHI_StructuredBuffer>(stride, 1, "lights");
	}

	std::string RendererPath::GetRenderPathName() const
	{
		switch (m_rendererPathType)
		{
		case RendererPathType_SceneView: return "SceneView";
		case RendererPathType_GameView: return "GameView";
		case RendererPathType_AssetView: return "AssetView";
		case RendererPathType_Custom: return "Custom";
		default:return "";
		}
	}

	bool RendererPath::CheckIsBuildInRendererCamera()
	{
		if (m_rendererPathType == RendererPathType_GameView || m_rendererPathType == RendererPathType_Custom)
		{
			return false;
		}

		return true;
	}

	void RendererPath::Update()
	{
		// TODO: TEST
		// m_renderScene->ResetResolve();

		UpdateSceneObject();

		// Sort
		FrustumCullAndSort(m_renderables[Renderer_Entity::Mesh]);

		UpdateLight();
	}

	void RendererPath::UpdateSceneObject()
	{
		if (!m_renderScene)
		{
			return;
		}

		// EASY_FUNCTION(profiler::colors::Magenta);
		if (!m_renderScene->IsNeedResolve())
		{
			return;
		}

		// clear previous state
		m_renderables.clear();
		m_renderables[Renderer_Entity::Mesh];
		m_renderables[Renderer_Entity::Camera];
		m_renderables[Renderer_Entity::Light];
		m_renderables[Renderer_Entity::UI];
		m_renderables[Renderer_Entity::Canvas];

		for (auto entity : m_renderScene->GetAllGameObjectList())
		{
			if (auto renderable = entity->GetComponent<SkinnedMeshRenderer>())
			{
				bool is_visible = true;

				/*if (const Material* material = renderable->GetMaterial())
				{
					is_transparent = material->GetProperty(MaterialProperty::ColorA) < 1.0f;
					is_visible = material->GetProperty(MaterialProperty::ColorA) != 0.0f;
				}*/

				if (is_visible)
				{
					m_renderables[Renderer_Entity::Mesh].emplace_back(entity);
				}
			}
			else
			{
				if (auto renderable = entity->GetComponent<MeshRenderer>())
				{
					bool is_visible = true;

					/*if (const Material* material = renderable->GetMaterial())
					{
						is_transparent = material->GetProperty(MaterialProperty::ColorA) < 1.0f;
						is_visible = material->GetProperty(MaterialProperty::ColorA) != 0.0f;
					}*/

					if (is_visible)
					{
						m_renderables[Renderer_Entity::Mesh].emplace_back(entity);
					}
				}
			}


			if (auto light = entity->GetComponent<Light>())
			{
				m_renderables[Renderer_Entity::Light].emplace_back(entity);
			}

			// remove Camera
			if (auto camera = entity->GetComponent<Camera>())
			{
				m_renderables[Renderer_Entity::Camera].emplace_back(entity);
			}

			if (auto text = entity->GetComponent<UIText>())
			{
				m_renderables[Renderer_Entity::UI].emplace_back(entity);
			}

			if (auto image = entity->GetComponent<UIImage>())
			{
				m_renderables[Renderer_Entity::UI].emplace_back(entity);
			}

			if (auto canvas = entity->GetComponent<UICanvas>())
			{
				m_renderables[Renderer_Entity::Canvas].emplace_back(entity);
			}

			/* if (auto reflection_probe = entity->GetComponent<ReflectionProbe>())
			 {
				 m_renderables[Renderer_Entity::ReflectionProbe].emplace_back(entity);
			 }

			 if (auto audio_source = entity->GetComponent<AudioSource>())
			 {
				 m_renderables[Renderer_Entity::AudioSource].emplace_back(entity);
			 }*/
		}

		// Reset
		m_renderScene->ResetResolve();
	}

	void RendererPath::UpdateLight()
	{
		if (m_rendererPathType == RendererPathType_AssetView)
		{
			UpdateDefaultLightBuffer();
		}
		else
		{

			if (GetLightCount() == 0)
			{
				m_mainLight = nullptr;
			}
			else
			{
				auto lightObject = this->m_renderables.at(Renderer_Entity::Light)[0];
				m_mainLight = lightObject->GetComponent<Light>();
			}

			if (!m_mainLight)
			{
				return;
			}

			// check need Create New ShadowMap
			if (CheckShadowMapNeedRecreate())
			{
				CreateShadowMap();
			}

			// Update Light Shader
			if (m_mainLight && m_mainLight->GetShadowsEnabled() && m_renderCamera)
			{
				ComputeLightViewMatrix();
				ComputeLightProjectionMatrix();
			}

			UpdateLightBuffer();
		}
	}

	const Matrix& RendererPath::GetLightViewMatrix(uint32_t index /*= 0*/) const
	{
		LC_ASSERT(index < static_cast<uint32_t>(m_matrix_view.size()));

		return m_matrix_view[index];
	}

	const Matrix& RendererPath::GetLightProjectionMatrix(uint32_t index /*= 0*/) const
	{
		LC_ASSERT(index < static_cast<uint32_t>(m_matrix_projection.size()));

		return m_matrix_projection[index];
	}

	uint32_t RendererPath::GetShadowArraySize() const
	{
		return m_shadow_map.texture_depth ? m_shadow_map.texture_depth->GetArrayLength() : 0;
	}

	void RendererPath::CreateShadowMap()
	{
		// Early exit if there is no change in shadow map resolution
		const uint32_t resolution = Renderer::GetOption<uint32_t>(Renderer_Option::ShadowResolution);

		if (m_shadow_map.texture_depth)
		{
			const bool resolution_changed = resolution != m_shadow_map.texture_depth->GetWidth();
			if (!resolution_changed)
				return;
		}

		// Early exit if this light casts no shadows
		if (!m_mainLight->GetShadowsEnabled())
		{
			m_shadow_map.texture_depth.reset();
			return;
		}
		bool shadowsTransparentEnabled = m_mainLight->GetShadowsTransparentEnabled();
		if (!shadowsTransparentEnabled)
		{
			m_shadow_map.texture_color.reset();
		}

		RHI_Format format_depth = RHI_Format::D32_Float;
		RHI_Format format_color = RHI_Format::R16G16B16A16_Float;// same other

		uint32_t flags_depth_buffer = RHI_Texture_Rtv | RHI_Texture_Srv | RHI_Texture_ClearBlit;
		if (m_mainLight->GetLightType() == LightType::Directional)
		{
			m_shadow_map.texture_depth = std::make_unique<RHI_Texture2DArray>(resolution, resolution, format_depth, 2, flags_depth_buffer, "shadow_map_directional");

			if (shadowsTransparentEnabled)
			{
				m_shadow_map.texture_color = std::make_unique<RHI_Texture2DArray>(resolution, resolution, format_color, 2, flags_depth_buffer, "shadow_map_directional_color");
			}

		}
		else if (m_mainLight->GetLightType() == LightType::Point)
		{
			m_shadow_map.texture_depth = std::make_unique<RHI_TextureCube>(resolution, resolution, format_depth, flags_depth_buffer, "shadow_map_point_color");

			if (shadowsTransparentEnabled)
			{
				m_shadow_map.texture_color = std::make_unique<RHI_TextureCube>(resolution, resolution, format_color, flags_depth_buffer, "shadow_map_point_color");
			}
		}
		else if (m_mainLight->GetLightType() == LightType::Spot)
		{
			m_shadow_map.texture_depth = std::make_unique<RHI_Texture2D>(resolution, resolution, 1, format_depth, flags_depth_buffer, "shadow_map_spot_color");

			if (shadowsTransparentEnabled)
			{
				m_shadow_map.texture_color = std::make_unique<RHI_Texture2D>(resolution, resolution, 1, format_color, flags_depth_buffer, "shadow_map_spot_color");
			}
		}
	}

	bool RendererPath::IsInLightViewFrustum(MeshFilter* renderable, uint32_t index) const
	{
		if (!m_mainLight)
		{
			return true;
		}

		BoundingBoxType type = renderable->HasInstancing() ? BoundingBoxType::TransformedInstances : BoundingBoxType::Transformed;
		const BoundingBox& box = renderable->GetBoundingBox(type);
		const auto center = box.GetCenter();
		const auto extents = box.GetExtents();

		// ensure that potential shadow casters from behind the near plane are not rejected
		const bool ignore_near_plane = (m_mainLight->GetLightType() == LightType::Directional) ? true : false;

		return m_frustums[index].IsVisible(center, extents, ignore_near_plane);
	}

	void RendererPath::UpdateLightBuffer()
	{
		if (GetLightCount() == 0)
		{
			return;
		}

		static std::array<Sb_Light, rhi_max_array_size_lights> properties;

		// clear
		properties.fill(Sb_Light{});

		const auto& lightEntities = GetRenderables().at(Renderer_Entity::Light);
		size_t lightCount = lightEntities.size();
		for (int index = 0; index < lightCount; index++)
		{
			const auto light = lightEntities[index]->GetComponent<Light>();

			// todo only one light(m_mainLight) has shadow
			for (uint32_t i = 0; i < GetShadowArraySize(); i++)
			{
				properties[index].view_projection[i] = GetLightViewMatrix(i) * GetLightProjectionMatrix(i);
			}

			properties[index].intensity = light->GetIntensityWatt(m_renderCamera);
			properties[index].color = light->GetColor();
			properties[index].range = light->GetRange();
			properties[index].angle = light->GetAngle();

			properties[index].position = light->GetGameObject()->GetComponent<Transform>()->GetPosition();
			properties[index].direction = light->GetGameObject()->GetComponent<Transform>()->GetForward();

			properties[index].flags = 0;
			properties[index].flags |= light->GetLightType() == LightType::Directional ? (1 << 0) : 0;
			properties[index].flags |= light->GetLightType() == LightType::Point ? (1 << 1) : 0;
			properties[index].flags |= light->GetLightType() == LightType::Spot ? (1 << 2) : 0;
			properties[index].flags |= light->GetShadowsEnabled() ? (1 << 3) : 0;
			properties[index].flags |= light->GetShadowsTransparentEnabled() ? (1 << 4) : 0;
			/*m_cb_light_arr_cpu.lightArr[index].flags |= light->GetShadowsScreenSpaceEnabled() ? (1 << 5) : 0;
			m_cb_light_arr_cpu.lightArr[index].flags |= light->GetVolumetricEnabled() ? (1 << 5) : 0;*/
		}

		// cpu to gpu
		uint32_t update_size = static_cast<uint32_t>(sizeof(Sb_Light)) * lightCount;
		m_light_structure_buffer->ResetOffset();
		m_light_structure_buffer->Update(&properties[0], update_size);
	}

	void RendererPath::UpdateDefaultLightBuffer()
	{
		static std::array<Sb_Light, rhi_max_array_size_lights> properties;
		int32_t index = 0;
		properties[index].intensity = 4.35f;
		properties[index].color = Color::White;
		properties[index].range = 200.0f;
		properties[index].angle = 0.5;

		properties[index].position = Vector3::Zero;
		properties[index].direction = Quaternion::FromAngleAxis(30.0f, Vector3::Forward) * Vector3::Forward;

		properties[index].flags = 0;
		properties[index].flags |= (1 << 0);

		// cpu to gpu
		uint32_t update_size = static_cast<uint32_t>(sizeof(Sb_Light)) * 1;
		m_light_structure_buffer->ResetOffset();
		m_light_structure_buffer->Update(&properties[0], update_size);
	}

	bool RendererPath::CheckShadowMapNeedRecreate()
	{
		if (!m_mainLight)
		{
			return false;
		}

		// Early exit if there is no change in shadow map resolution
		bool resolution_changed = false;
		const uint32_t resolution = Renderer::GetOption<uint32_t>(Renderer_Option::ShadowResolution);
		if(m_shadow_map.texture_depth)
		{
			resolution_changed = resolution != m_shadow_map.texture_depth->GetWidth();
		}

		if (m_mainLight->GetShadowsEnabled() == m_last_shadows_enabled &&
			m_mainLight->GetShadowsTransparentEnabled() == m_last_shadows_transparent_enabled && !resolution_changed)
		{
			return false;
		}


		m_last_shadows_enabled = m_mainLight->GetShadowsEnabled();
		m_last_shadows_transparent_enabled = m_mainLight->GetShadowsTransparentEnabled();

		return true;
	}

	void RendererPath::ComputeLightViewMatrix()
	{
		auto lightObject = m_mainLight->GetGameObject();
		const Vector3 position = lightObject->GetComponent<Transform>()->GetPosition();
		const Vector3 forward = lightObject->GetComponent<Transform>()->GetForward();
		const Vector3 up = lightObject->GetComponent<Transform>()->GetUp();

		if (m_mainLight->GetLightType() == LightType::Directional)
		{
			if (m_renderCamera)
			{
				Vector3 target = m_renderCamera->GetPosition();

				// near cascade
				Vector3 position = target - forward * orthographic_depth * 0.8f;
				m_matrix_view[0] = Matrix::CreateLookAtLH(position, target, Vector3::Up);

				// far cascade
				m_matrix_view[1] = m_matrix_view[0];
			}
		}
		else if (m_mainLight->GetLightType() == LightType::Spot)
		{
			// Compute
			m_matrix_view[0] = Matrix::CreateLookAtLH(position, position + forward, up);
		}
		else if (m_mainLight->GetLightType() == LightType::Point)
		{
			// Compute view for each side of the cube map
			m_matrix_view[0] = Matrix::CreateLookAtLH(position, position + Vector3::Right, Vector3::Up);       // x+
			m_matrix_view[1] = Matrix::CreateLookAtLH(position, position + Vector3::Left, Vector3::Up);       // x-
			m_matrix_view[2] = Matrix::CreateLookAtLH(position, position + Vector3::Up, Vector3::Backward); // y+
			m_matrix_view[3] = Matrix::CreateLookAtLH(position, position + Vector3::Down, Vector3::Forward);  // y-
			m_matrix_view[4] = Matrix::CreateLookAtLH(position, position + Vector3::Forward, Vector3::Up);       // z+
			m_matrix_view[5] = Matrix::CreateLookAtLH(position, position + Vector3::Backward, Vector3::Up);       // z-
		}
	}

	void RendererPath::ComputeLightProjectionMatrix()
	{
		if (m_mainLight->GetLightType() == LightType::Directional)
		{
			for (uint32_t i = 0; i < 2; i++)
			{
				// determine the orthographic extent based on the cascade index
				float extent = (i == 0) ? orthographic_extent_near : orthographic_extent_far;

				// orthographic bounds
				float left = -extent;
				float right = extent;
				float bottom = -extent;
				float top = extent;
				float near_plane = 0.0f;
				float far_plane = orthographic_depth;

				m_matrix_projection[i] = Matrix::CreateOrthoOffCenterLH(left, right, bottom, top, far_plane, near_plane);
				m_frustums[i] = Frustum(m_matrix_view[i], m_matrix_projection[i], far_plane - near_plane);
			}
		}
		else
		{
			const float aspect_ratio = static_cast<float>(m_shadow_map.texture_depth->GetWidth()) / static_cast<float>(m_shadow_map.texture_depth->GetHeight());
			const float fov = m_mainLight->GetLightType() == LightType::Spot ? m_mainLight->GetAngle() * 2.0f : Math::Helper::PI_DIV_2;
			Matrix projection = Matrix::CreatePerspectiveFieldOfViewLH(fov, aspect_ratio, m_mainLight->GetRange(), 0.3f);

			for (uint32_t i = 0; i < m_shadow_map.texture_depth->GetArrayLength(); i++)
			{
				m_matrix_projection[i] = projection;
				m_frustums[i] = Frustum(m_matrix_view[i], projection, m_mainLight->GetRange());
			}
		}
	}

	float RendererPath::GetSquaredDistance(const GameObject* entity)
	{
		Vector3 camera_position = m_renderCamera->GetPosition();
		uint64_t entity_id = entity->GetObjectId();

		auto it = distances_squared.find(entity_id);
		if (it != distances_squared.end())
		{
			return it->second;
		}
		else
		{
			auto* renderable = entity->GetComponent<MeshFilter>();
			BoundingBoxType type = renderable->HasInstancing() ? BoundingBoxType::TransformedInstances : BoundingBoxType::Transformed;
			Vector3 position = renderable->GetBoundingBox(type).GetCenter();
			float distance_squared = (position - camera_position).LengthSquared();
			distances_squared[entity_id] = distance_squared;

			return distance_squared;
		}
	}

	void RendererPath::FrustumCulling(std::vector<GameObject*>& renderables)
	{
		/*for (auto entity : renderables)
		{
			shared_ptr<Renderable> renderable = entity->GetComponent<Renderable>();
			renderable->SetFlag(RenderableFlags::OccludedCpu, !Renderer::GetCamera()->IsInViewFrustum(renderable));
			renderable->SetFlag(RenderableFlags::Occluder, false);
		}*/
	}

	void RendererPath::Sort(std::vector<GameObject*>& renderables)
	{ // 1. sort by depth
		sort(renderables.begin(), renderables.end(), [this](GameObject* a, const GameObject* b)
			{
				//// skip entities which are outside of the view frustum
				//if (a->GetComponent<Renderable>()->HasFlag(OccludedCpu) || b->GetComponent<Renderable>()->HasFlag(OccludedCpu))
				//	return false;

				// front-to-back for opaque (todo, handle inverse sorting for transparents)
				return GetSquaredDistance(a) < GetSquaredDistance(b);
			});

		// 2. sort by instancing, instanced objects go to the front
		stable_sort(renderables.begin(), renderables.end(), [](GameObject* a, const GameObject* b)
			{
				return a->GetComponent<MeshFilter>()->HasInstancing() > b->GetComponent<MeshFilter>()->HasInstancing();
			});

		// 3. sort by transparency, transparent materials go to the end
		stable_sort(renderables.begin(), renderables.end(), [](GameObject* a, const GameObject* b)
			{
				bool a_transparent = a->GetComponent<MeshRenderer>()->GetMaterial()->IsTransparent();
				bool b_transparent = b->GetComponent<MeshRenderer>()->GetMaterial()->IsTransparent();

				// non-transparent objects should come first, so invert the condition
				return !a_transparent && b_transparent;
			});
	}

	void RendererPath::FrustumCullAndSort(std::vector<GameObject*>& renderables)
	{
		FrustumCulling(renderables);
		Sort(renderables);

		// find transparent index
		auto transparent_start = find_if(renderables.begin(), renderables.end(), [](GameObject* entity)
			{
				return entity->GetComponent<MeshRenderer>()->GetMaterial()->IsTransparent();
			});
		m_meshIndexTransparent = distance(renderables.begin(), transparent_start);

		// find non-instanced index for opaque objects
		auto non_instanced_opaque_start = find_if(renderables.begin(), renderables.end(), [&](GameObject* entity)
			{
				MeshRenderer* meshRenderer = entity->GetComponent<MeshRenderer>();
				bool is_transparent = meshRenderer->GetMaterial()->IsTransparent();

				MeshFilter* meshFilter = entity->GetComponent<MeshFilter>();
				bool is_instanced = meshFilter->HasInstancing();

				return !is_transparent && !is_instanced;
			});
		m_meshIndexNonInstancedOpaque = distance(renderables.begin(), non_instanced_opaque_start);

		// find non-instanced index for transparent objects
		auto non_instanced_transparent_start = find_if(transparent_start, renderables.end(), [&](GameObject* entity)
			{
				return !entity->GetComponent<MeshFilter>()->HasInstancing();
			});
		m_meshIndexNonInstancedTransparent = distance(renderables.begin(), non_instanced_transparent_start);
	}
}
