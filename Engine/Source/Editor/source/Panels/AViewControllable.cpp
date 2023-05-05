/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "Editor/include/Panels/AViewControllable.h"

#include <string>

LitchiEditor::AViewControllable::AViewControllable
(
	const std::string& p_title,
	bool p_opened,
	const PanelWindowSettings& p_windowSettings,
	bool p_enableFocusInputs
) : AView(p_title, p_opened, p_windowSettings), m_cameraController(*this, m_camera, m_cameraPosition, m_cameraRotation, p_enableFocusInputs)
{

}

void LitchiEditor::AViewControllable::Update(float p_deltaTime)
{
	m_cameraController.HandleInputs(p_deltaTime);

	AView::Update(p_deltaTime);
}

LitchiEditor::CameraController& LitchiEditor::AViewControllable::GetCameraController()
{
	return m_cameraController;
}
