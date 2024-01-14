
#pragma once

#include <string>

#include "AViewControllable.h"
#include "Runtime/Function/UI/Settings/PanelWindowSettings.h"

#include "Runtime/Function/Renderer/Rendering/Renderer.h"

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
			RendererPath* rendererPath
		);

		/**
		* Update the scene view
		*/
		virtual void UpdateView(float p_deltaTime) override;

		/**
		* Custom implementation of the render method
		*/
		virtual void _Render_Impl() override;

		/**
		* OnDraw
		*/
		virtual void OnDraw() override;

		/**
		* Render the scene for actor picking (Using unlit colors)
		*/
		void RenderSceneForActorPicking();

		/**
		* Render the scene for actor picking and handle the logic behind it
		*/
		void HandleActorPicking();
	private:
		GameObject* m_highlightedActor;
		TransformGizmo* m_transform_gizmo;
	};
}
