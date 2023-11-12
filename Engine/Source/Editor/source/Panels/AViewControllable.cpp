
#include "Editor/include/Panels/AViewControllable.h"

#include <string>
#include "Runtime/Function/Renderer/RenderCamera.h"

LitchiEditor::AViewControllable::AViewControllable
(
	const std::string& p_title,
	bool p_opened,
	const PanelWindowSettings& p_windowSettings,
	RendererPath* rendererPath,
	bool p_enableFocusInputs
) : AView(p_title, p_opened, p_windowSettings, rendererPath),
	m_cameraController(*this, m_camera, m_cameraPosition, m_cameraRotation, p_enableFocusInputs)
{

}

void LitchiEditor::AViewControllable::Update(float p_deltaTime)
{
	// 1. 更新cameraCtrl
	m_cameraController.HandleInputs(p_deltaTime);

	/*m_camera->SetPosition(m_cameraController.GetPosition());
	m_camera->SetRotation(m_cameraController.GetRotation());*/

	// 2. 更新View
	AView::Update(p_deltaTime);
}

LitchiEditor::CameraController& LitchiEditor::AViewControllable::GetCameraController()
{
	return m_cameraController;
}
