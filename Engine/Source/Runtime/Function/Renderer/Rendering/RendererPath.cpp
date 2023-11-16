#include "RendererPath.h"

#include <memory>

#include "Runtime/Function/Framework/Component/Camera/camera.h"
#include "Runtime/Function/Framework/Component/Light/Light.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshRenderer.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Renderer/RHI/RHI_Texture.h"
#include "Runtime/Function/Renderer/RHI/RHI_Texture2D.h"

namespace LitchiRuntime
{
	void sort_renderables(RenderCamera* camera, std::vector<GameObject*>* renderables, const bool are_transparent)
	{
		if (!camera || renderables->size() <= 2)
			return;

		auto comparison_op = [camera](GameObject* entity)
		{
			auto renderable = entity->GetComponent<MeshFilter>();
			if (!renderable)
				return 0.0f;

			return (renderable->GetAabb().GetCenter() - camera->GetPosition()).LengthSquared();
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

	RendererPath::RendererPath(RendererPathType rendererPathType, RenderCamera* renderCamera)
	{
		m_rendererPathType = rendererPathType;
		m_renderCamera = renderCamera;

		m_width = m_renderCamera->GetViewport().width;
		m_height = m_renderCamera->GetViewport().height;

		m_renderables.clear();

		CreateColorRenderTarget();
	}

	void RendererPath::UpdateRenderTarget(float width, float height)
	{
		m_width = width;
		m_height = height;

		CreateColorRenderTarget();
	}

	void RendererPath::CreateColorRenderTarget()
	{
		std::string rtName = GetRenderPathName() + std::string("_frame_output");
		m_colorRenderTarget = std::make_shared<RHI_Texture2D>(m_width, m_height, 1, RHI_Format::R16G16B16A16_Float, RHI_Texture_RenderTarget | RHI_Texture_Uav | RHI_Texture_Srv | RHI_Texture_ClearOrBlit, rtName.c_str());

	}

	void RendererPath::CreateDepthRenderTarget()
	{
		std::string rtName = GetRenderPathName() + std::string("_frame_depth");
		static float resolutionWidth = 4096;
		static float resolutionHeight = 4096;
		m_depthRenderTarget = std::make_shared<RHI_Texture2D>(resolutionWidth, resolutionHeight, 1, RHI_Format::D32_Float, RHI_Texture_RenderTarget | RHI_Texture_Srv, rtName.c_str());
	}

	std::string RendererPath::GetRenderPathName()
	{
		switch (m_rendererPathType)
		{
		case RendererPathType_SceneView: return "SceneView";
		case RendererPathType_GameView: return "GameView";
		case RendererPathType_Custom: return "Custom";
		default: ;
		}
	}

	void RendererPath::UpdateRenderableGameObject()
	{
		// clear previous state
		m_renderables.clear();

		for (auto entity : m_renderScene->GetAllGameObjectList())
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

			if (auto light = entity->GetComponent<Light>())
			{
				m_renderables[Renderer_Entity::Light].emplace_back(entity);
			}

			// remove Camera
			if (auto camera = entity->GetComponent<Camera>())
			{
				m_renderables[Renderer_Entity::Camera].emplace_back(entity);
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

		// sort them by distance
		sort_renderables(m_renderCamera, &m_renderables[Renderer_Entity::Geometry], false);
		sort_renderables(m_renderCamera, &m_renderables[Renderer_Entity::GeometryTransparent], true);
	}
	
}
