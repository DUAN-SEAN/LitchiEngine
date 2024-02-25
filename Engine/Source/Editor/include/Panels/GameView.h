
#pragma once

#include <string>
#include "AViewControllable.h"
#include "Runtime/Function/UI/Settings/PanelWindowSettings.h"
#include "Runtime/Function/Renderer/Rendering/Renderer.h"

namespace LitchiEditor
{
	class GameView : public AView
	{
	public:
		/**
		* Constructor
		* @param p_title
		* @param p_opened
		* @param p_windowSettings
		* @param p_renderTargetTexture
		*/
		GameView
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
		
	private:
		GameObject* m_highlightedActor;
		TransformGizmo* m_transform_gizmo;
	};
}
