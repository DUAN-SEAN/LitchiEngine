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
		float orthographic_extent_near = 64.0f;
		float orthographic_extent_far = 256.0f;
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

		//CreateLightBuffer();
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

		if (m_selectedMesh_bone_constant_buffer)
		{
			m_selectedMesh_bone_constant_buffer.reset();
		}
	}

	void RendererPath::SetScene(Scene* scene)
	{
		m_renderScene = scene;
		//m_mainLight = nullptr;
		m_needUpdateRenderScene = true;
	}

	bool RendererPath::HasRenderable(Renderer_Entity rendererEntity)
	{
		if (this->m_renderables.find(rendererEntity) != m_renderables.end())
		{
			return true;
		}

		return false;
	}

	bool RendererPath::HasTransparentMesh()
	{
		if (this->m_renderables.find(Renderer_Entity::Mesh) == m_renderables.end())
		{
			return false;
		}

		auto& meshEntityList = m_renderables[Renderer_Entity::Mesh];
		if (meshEntityList.size() == 0)
		{
			return false;
		}

		if (meshEntityList.size() == m_meshIndexTransparent)
		{
			return false;
		}

		return true;
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

	size_t RendererPath::GetLightGameObjectCount() const
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

		if (!CheckIsBuildInRendererCamera() && HasRenderable(Renderer_Entity::Camera))
		{
			auto cameraObject = m_renderables[Renderer_Entity::Camera][0];
			auto camera = cameraObject->GetComponent<Camera>();
			SetRenderCamera(camera->GetRenderCamera());
		}

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
		if (!m_needUpdateRenderScene && !m_renderScene->IsNeedResolve())
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
		m_needUpdateRenderScene = false;
	}

	bool RendererPath::CheckNeedRebuildRendererLightArr(int32_t lightGameObjectCount)
	{
		if(lightGameObjectCount<=0)
		{
			return false;
		}

		for (int32_t index = 0; index < lightGameObjectCount; index++)
		{
			auto lightObject = m_renderables[Renderer_Entity::Light][index];
			auto tempLight = lightObject->GetComponent<Light>();

			auto& rendererLight = m_rendererLightGroup.m_light_arr[index];
			if (rendererLight.m_light->GetObjectId() != tempLight->GetObjectId() ||
				rendererLight.m_light->GetLightType() != tempLight->GetLightType() ||
				rendererLight.m_light->GetAngle() != tempLight->GetAngle() ||
				rendererLight.m_light->GetRange() != tempLight->GetRange()
				)
			{
				return true;
			}
		}

		return false;
	}

	bool RendererPath::CheckNeedRebuildLightBuffer(int32_t lightGameObjectCount)
	{
		if (m_rendererLightGroup.m_light_arr.size() != lightGameObjectCount && lightGameObjectCount > 0)
		{
			return true;
		}

		return false;
	}

	void RendererPath::FillRendererLightData(RendererLightData& lightData)
	{
		const float near_plane = 0.01f;

		auto light = lightData.m_light;
		const Vector3 position = light->GetGameObject()->GetComponent<Transform>()->GetPosition();
		const Vector3 forward = light->GetGameObject()->GetComponent<Transform>()->GetForward();
		switch (light->GetLightType())
		{
		case LightType::Directional:
		{

			// View matrix
			Vector3 target = Vector3::Zero;
			if (m_renderCamera)
			{
				target = m_renderCamera->GetPosition();
			}
			Vector3 directinalDeployPosition = target - forward * orthographic_depth * 0.8f;
			lightData.m_matrix_view[0] = Matrix::CreateLookAtLH(directinalDeployPosition, target, Vector3::Up); // near
			lightData.m_matrix_view[1] = lightData.m_matrix_view[0];                                      // far

			// Projection matrix
			for (uint32_t i = 0; i < 2; i++)
			{
				// determine the orthographic extent based on the cascade index
				float extent = (i == 0) ? orthographic_extent_near : orthographic_extent_far;

				// orthographic bounds
				float left = -extent;
				float right = extent;
				float bottom = -extent;
				float top = extent;
				float far_plane = orthographic_depth;

				lightData.m_matrix_projection[i] = Matrix::CreateOrthoOffCenterLH(left, right, bottom, top, far_plane, near_plane);
				lightData.m_frustums[i] = Frustum(lightData.m_matrix_view[i], lightData.m_matrix_projection[i], far_plane - near_plane);
			}

		}
		break;
		case LightType::Point:
		{
			// View matrix
			lightData.m_matrix_view[0] = Matrix::CreateLookAtLH(position, position + forward, Vector3::Up); // front paraboloid
			lightData.m_matrix_view[1] = Matrix::CreateLookAtLH(position, position - forward, Vector3::Up); // back paraboloid

			//const float aspect_ratio = static_cast<float>(m_shadow_map.texture_depth->GetWidth()) / static_cast<float>(m_shadow_map.texture_depth->GetHeight());
			const float aspect_ratio = 1.0f;
			const float fov = light->GetAngle() * 2.0f;
			Matrix projection = Matrix::CreatePerspectiveFieldOfViewLH(fov, aspect_ratio, light->GetRange(), near_plane);

			lightData.m_matrix_projection[0] = projection;
			lightData.m_frustums[0] = Frustum(lightData.m_matrix_view[0], projection, light->GetRange());
		}
		break;
		case LightType::Spot:
		{
			// View matrix
			lightData.m_matrix_view[0] = Matrix::CreateLookAtLH(position, position + forward, Vector3::Up);

			//const float aspect_ratio = static_cast<float>(m_shadow_map.texture_depth->GetWidth()) / static_cast<float>(m_shadow_map.texture_depth->GetHeight());
			const float aspect_ratio = 1.0f;
			const float fov = light->GetAngle() * 2.0f;
			Matrix projection = Matrix::CreatePerspectiveFieldOfViewLH(fov, aspect_ratio, light->GetRange(), near_plane);

			lightData.m_matrix_projection[0] = projection;
			lightData.m_frustums[0] = Frustum(lightData.m_matrix_view[0], projection, light->GetRange());
		}

		break;
		}
	}

	void RendererPath::UpdateLight()
	{
		if (m_rendererPathType == RendererPathType_AssetView)
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
			if (m_rendererLightGroup.m_light_structure_buffer == nullptr)
			{
				uint32_t stride = static_cast<uint32_t>(sizeof(Sb_Light)) * rhi_max_array_size_lights;
				m_rendererLightGroup.m_light_structure_buffer = std::make_shared<RHI_StructuredBuffer>(stride, 1, "lights");
			}
			m_rendererLightGroup.m_light_structure_buffer->ResetOffset();
			m_rendererLightGroup.m_light_structure_buffer->Update(&properties[0], update_size);
		}
		else
		{


			const uint32_t resolution = 4096;// todo: 
			//const uint32_t resolution = Renderer::GetOption<uint32_t>(Renderer_Option::ShadowResolution);


			int32_t lightGameObjectCount = GetLightGameObjectCount();
			bool needRebuildLightBuffer = CheckNeedRebuildLightBuffer(lightGameObjectCount);
			// check need rebuild buffer
			if (needRebuildLightBuffer)
			{
				// create shadow map
				RHI_Format format_depth = RHI_Format::D32_Float;
				RHI_Format format_color = RHI_Format::R16G16B16A16_Float;// same other
				uint32_t flags = RHI_Texture_Rtv | RHI_Texture_Srv | RHI_Texture_ClearBlit;
				auto lightCount = lightGameObjectCount;
				if (m_rendererLightGroup.m_texture_depth == nullptr || m_rendererLightGroup.m_texture_depth->GetArrayLength() != 2 * lightCount)
				{
					if (m_rendererLightGroup.m_texture_depth)
					{
						m_rendererLightGroup.m_texture_depth.reset();
					}
					m_rendererLightGroup.m_texture_depth = std::make_unique<RHI_Texture2DArray>(
						resolution, resolution, format_depth, 2 * lightCount, flags, GetRenderPathName() + "_light_depth_arr");

					if (m_rendererLightGroup.m_texture_color)
					{
						m_rendererLightGroup.m_texture_color.reset();
					}
					//if (shadowsTransparentEnabled)
					{
						m_rendererLightGroup.m_texture_color = std::make_unique<RHI_Texture2DArray>(
							resolution, resolution, format_color, 2 * lightCount, flags, GetRenderPathName() + "_light_color_arr");
					}
				}

			}

			bool needRebuildRendererLightArr = needRebuildLightBuffer || CheckNeedRebuildRendererLightArr(lightGameObjectCount);
			if(needRebuildRendererLightArr)
			{
				m_rendererLightGroup.m_light_arr.clear();
				auto& lightObjectArr = m_renderables[Renderer_Entity::Light];
				int lightCount = 0;
				for (auto lightObject : lightObjectArr)
				{
					/*if(lightCount> rhi_max_array_size_lights)
					{
						break;
					}*/

					auto light = lightObject->GetComponent<Light>();
					RendererLightData rendererLightData{};
					// set light
					rendererLightData.m_light = light;
					rendererLightData.m_shadow_count = 2;
					FillRendererLightData(rendererLightData);

					// add to light cache
					m_rendererLightGroup.m_light_arr.push_back(rendererLightData);
					lightCount++;
				}
			}else
			{
				for (RendererLightData& rendererLightData : m_rendererLightGroup.m_light_arr)
				{
					FillRendererLightData(rendererLightData);
				}
			}

			
			// update light buffer
			static std::array<Sb_Light, rhi_max_array_size_lights> properties;

			// clear
			properties.fill(Sb_Light{});
			size_t lightCount = m_rendererLightGroup.GetLightCount();
			for (int index = 0; index < lightCount; index++)
			{
				const auto& rendererLightData = m_rendererLightGroup.m_light_arr[index];
				const auto light = rendererLightData.m_light;

				for (uint32_t i = 0; i < rendererLightData.m_shadow_count; i++)
				{
					if (light->GetLightType() == LightType::Point)
					{
						// we do paraboloid projection in the vertex shader so we only want the view here
						properties[index].view_projection[i] = rendererLightData.GetLightViewMatrix(i);
					}
					else
					{
						properties[index].view_projection[i] = rendererLightData.GetLightViewMatrix(i) * rendererLightData.GetLightProjectionMatrix(i);
					}
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
			uint32_t update_size = static_cast<uint32_t>(sizeof(Sb_Light)) * lightGameObjectCount;
			if (m_rendererLightGroup.m_light_structure_buffer == nullptr)
			{
				uint32_t stride = static_cast<uint32_t>(sizeof(Sb_Light)) * rhi_max_array_size_lights;
				m_rendererLightGroup.m_light_structure_buffer = std::make_shared<RHI_StructuredBuffer>(stride, 1, "lights");
			}
			m_rendererLightGroup.m_light_structure_buffer->ResetOffset();
			m_rendererLightGroup.m_light_structure_buffer->Update(&properties[0], update_size);

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
