#include "RendererPath.h"

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

	void sort_renderables(RenderCamera* camera, std::vector<GameObject*>* renderables, const bool are_transparent)
	{
		if (!camera || renderables->size() <= 2)
			return;

		auto comparison_op = [camera](GameObject* entity)
			{
				auto renderable = entity->GetComponent<MeshFilter>();
				if (!renderable)
					return 0.0f;

				return (renderable->GetAAbb().GetCenter() - camera->GetPosition()).LengthSquared();
			};

		// sort by depth
		sort(renderables->begin(), renderables->end(), [&comparison_op, &are_transparent](GameObject* a, GameObject* b)
			{
				if (are_transparent)
				{
					return comparison_op(a) > comparison_op(b); // back-to-front for transparent
				}
				else
				{
					return comparison_op(a) < comparison_op(b); // front-to-back for opaque
				}
			});
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

		if(m_selectedMesh_bone_constant_buffer)
		{
			m_selectedMesh_bone_constant_buffer.reset();
		}
	}

	void RendererPath::UpdateScene(Scene* scene)
	{
		m_renderScene = scene;
		m_mainLight = nullptr;
	}

	void RendererPath::UpdateRenderCamera(RenderCamera* camera)
	{
		if(CheckIsBuildInRendererCamera())
		{
			return;
		}

		if(camera ==nullptr || m_renderCamera == camera)
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


	void RendererPath::UpdateRenderTarget(float width, float height)
	{
		m_width = width;
		m_height = height;

		CreateColorRenderTarget();
	}

	UICanvas* RendererPath::GetCanvas()
	{
		if (this->m_renderables.find(Renderer_Entity::Canvas) !=	m_renderables.end())
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

	void RendererPath::UpdateSelectedAssetViewResource(Material* material, Mesh* mesh, RHI_Texture2D* texture_2d)
	{
		// only one select
		m_selectedMaterial = material;
		m_selectedMesh = mesh;
		m_selectedTexture2D = texture_2d;

		if(m_selectedMaterial != nullptr)
		{
			m_selectedResType = SelectedResourceType_Material;
			m_selectedMesh_bone_constant_buffer.reset();
			m_selectedMesh_bone_constant_buffer = nullptr;
		}
		else if(m_selectedMesh != nullptr)
		{
			m_selectedResType = SelectedResourceType_Mesh;

			if(m_selectedMesh_bone_constant_buffer == nullptr)
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
		else if(m_selectedTexture2D != nullptr)
		{
			m_selectedResType = SelectedResourceType_Texture2D;
			m_selectedMesh_bone_constant_buffer.reset();
			m_selectedMesh_bone_constant_buffer = nullptr;
		}
	}

	void RendererPath::CreateColorRenderTarget()
	{
		// typical flags
		uint32_t flags_standard = RHI_Texture_Uav | RHI_Texture_Srv;
		uint32_t flags_render_target = flags_standard | RHI_Texture_Rtv;

		std::string rtName = GetRenderPathName() + std::string("_frame_output");
		m_colorRenderTarget = std::make_shared<RHI_Texture2D>(m_width, m_height, 1, RHI_Format::R16G16B16A16_Float, flags_render_target | RHI_Texture_ClearBlit , rtName.c_str());

	}

	void RendererPath::CreateDepthRenderTarget()
	{
		uint32_t flags_depth_buffer = RHI_Texture_Rtv | RHI_Texture_Srv;
		std::string rtName = GetRenderPathName() + std::string("_frame_depth");

		static float resolutionWidth = 4096;
		static float resolutionHeight = 4096;
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

	void RendererPath::UpdateRenderableGameObject()
	{
		if(!m_renderScene)
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
		m_renderables[Renderer_Entity::Geometry];
		m_renderables[Renderer_Entity::GeometryTransparent];
		m_renderables[Renderer_Entity::SkinGeometry];
		m_renderables[Renderer_Entity::SkinGeometryTransparent];
		m_renderables[Renderer_Entity::Camera];
		m_renderables[Renderer_Entity::Light];
		m_renderables[Renderer_Entity::UI];
		m_renderables[Renderer_Entity::Canvas];

		for (auto entity : m_renderScene->GetAllGameObjectList())
		{
			if (auto renderable = entity->GetComponent<SkinnedMeshRenderer>())
			{
				bool is_transparent = false;
				bool is_visible = true;

				/*if (const Material* material = renderable->GetMaterial())
				{
					is_transparent = material->GetProperty(MaterialProperty::ColorA) < 1.0f;
					is_visible = material->GetProperty(MaterialProperty::ColorA) != 0.0f;
				}*/

				if (is_visible)
				{
					m_renderables[is_transparent ? Renderer_Entity::SkinGeometryTransparent : Renderer_Entity::SkinGeometry].emplace_back(entity);
				}
			}else
			{
				if (auto renderable = entity->GetComponent<MeshRenderer>())
				{
					bool is_transparent = false;
					bool is_visible = true;

					/*if (const Material* material = renderable->GetMaterial())
					{
						is_transparent = material->GetProperty(MaterialProperty::ColorA) < 1.0f;
						is_visible = material->GetProperty(MaterialProperty::ColorA) != 0.0f;
					}*/

					if (is_visible)
					{
						m_renderables[is_transparent ? Renderer_Entity::GeometryTransparent : Renderer_Entity::Geometry].emplace_back(entity);
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

			if(auto text = entity->GetComponent<UIText>())
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

		// TODO: TEST
		// m_renderScene->ResetResolve();

		// sort them by distance
		sort_renderables(m_renderCamera, &m_renderables[Renderer_Entity::Geometry], false);
		sort_renderables(m_renderCamera, &m_renderables[Renderer_Entity::GeometryTransparent], true);
	}

	void RendererPath::UpdateLightShadow()
	{
		if(this->m_renderables.size()>0)
		{
			auto& lights = this->m_renderables.at(Renderer_Entity::Light);
			if(!lights.empty())
			{
				auto lightObject = this->m_renderables.at(Renderer_Entity::Light)[0];
				m_mainLight = lightObject->GetComponent<Light>();
			}else
			{
				// todo clear?
				m_mainLight = nullptr;
			}
		}

		if(!m_mainLight)
		{
			return;
		}

		// check need Create New ShadowMap
		if(CheckShadowMapNeedRecreate())
		{
			CreateShadowMap();
		}

		// Update Light Shader
		if(m_mainLight && m_mainLight->GetShadowsEnabled() && m_renderCamera)
		{
			ComputeLightViewMatrix();
			ComputeLightProjectionMatrix();
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

		if(m_shadow_map.texture_depth)
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
		RHI_Format format_color = RHI_Format::R8G8B8A8_Unorm;

		uint32_t flags_depth_buffer = RHI_Texture_Rtv | RHI_Texture_Srv;
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
		if(!m_mainLight)
		{
			return true;
		}

		const auto box = renderable->GetAAbb();
		const auto center = box.GetCenter();
		const auto extents = box.GetExtents();

		// ensure that potential shadow casters from behind the near plane are not rejected
		const bool ignore_near_plane = (m_mainLight->GetLightType() == LightType::Directional) ? true : false;

		return m_frustums[index].IsVisible(center, extents, ignore_near_plane);
	}

	bool RendererPath::CheckShadowMapNeedRecreate()
	{
		if(!m_mainLight)
		{
			return false;
		}

		if(m_mainLight->GetShadowsEnabled() == m_last_shadows_enabled &&
			m_mainLight->GetShadowsTransparentEnabled() == m_last_shadows_transparent_enabled)
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

		if (m_mainLight->GetLightType()== LightType::Directional)
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

}
