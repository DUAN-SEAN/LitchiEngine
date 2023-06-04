
#pragma once

#include <string>

#include "AViewControllable.h"
#include "Editor/include/Core/GizmoBehaviour.h"
#include "Runtime/Function/Scene/scene_manager.h"
#include "Runtime/Function/UI/Settings/PanelWindowSettings.h"
#include <optional>

#include "Runtime/Function/Renderer/Resources/Material.h"

namespace LitchiEditor
{
	class SceneView : public AViewControllable
	{
	public:
		/**
		* Constructor
		* @param p_title
		* @param p_opened
		* @param p_windowSettings
		*/
		SceneView
		(
			const std::string& p_title,
			bool p_opened,
			const PanelWindowSettings& p_windowSettings
		);

		/**
		* Update the scene view
		*/
		virtual void Update(float p_deltaTime) override;

		/**
		* Custom implementation of the render method
		*/
		virtual void _Render_Impl() override;

		/**
		* Render the actual scene
		* @param p_defaultRenderState
		*/
		void RenderScene();

		/**
		* Render the scene for actor picking (Using unlit colors)
		*/
		void RenderSceneForActorPicking();

		/**
		* Render the scene for actor picking and handle the logic behind it
		*/
		void HandleActorPicking();
	private:
		void RenderGrid(const glm::vec3& p_viewPos, const glm::vec3& p_color);
	private:
		Framebuffer m_actorPickingFramebuffer;
		GizmoBehaviour m_gizmoOperations;
		EGizmoOperation m_currentOperation = EGizmoOperation::TRANSLATE;

		// std::optional<std::reference_wrapper<OvCore::ECS::Actor>> m_highlightedActor;
		GameObject* m_highlightedActor;
		std::optional<GizmoBehaviour::EDirection> m_highlightedGizmoDirection;


		Resource::Material m_gridMaterial;
	};
}
