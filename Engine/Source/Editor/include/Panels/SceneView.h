
#pragma once

#include <string>

#include "AViewControllable.h"
#include "Editor/include/Core/GizmoBehaviour.h"
#include "Runtime/Function/Scene/SceneManager.h"
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
		* @param p_renderTargetTexture
		*/
		SceneView
		(
			const std::string& p_title,
			bool p_opened,
			const PanelWindowSettings& p_windowSettings,
			RHI_Texture* renderTargetTexture
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
		* Render the scene for actor picking (Using unlit colors)
		*/
		void RenderSceneForActorPicking();

		/**
		* Render the scene for actor picking and handle the logic behind it
		*/
		void HandleActorPicking();
	private:
	private:
		/*Framebuffer m_actorPickingFramebuffer;*/
		GizmoBehaviour m_gizmoOperations;
		EGizmoOperation m_currentOperation = EGizmoOperation::TRANSLATE;

		// std::optional<std::reference_wrapper<GameObject>> m_highlightedActor;
		GameObject* m_highlightedActor;
		std::optional<GizmoBehaviour::EDirection> m_highlightedGizmoDirection;
	};
}
