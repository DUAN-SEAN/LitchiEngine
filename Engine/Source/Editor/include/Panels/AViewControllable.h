
#pragma once

#include "AView.h"
#include "Editor/include/Core/CameraController.h"

namespace LitchiEditor
{
	class AViewControllable : public AView
	{
	public:
		/**
		* Constructor
		* @param p_title
		* @param p_opened
		* @param p_windowSettings
		* @param p_enableFocusInputs
		*/
		AViewControllable
		(
			const std::string& p_title,
			bool p_opened,
			const PanelWindowSettings& p_windowSettings,
			bool p_enableFocusInputs = false
		);

		/**
		* Update the controllable view (Handle inputs)
		* @param p_deltaTime
		*/
		virtual void Update(float p_deltaTime) override;

		/**
		* Returns the camera controller of the controllable view
		*/
		CameraController& GetCameraController();

	protected:
		CameraController m_cameraController;
	};
}
