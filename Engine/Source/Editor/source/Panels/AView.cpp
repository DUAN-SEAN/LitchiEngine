

#include "Editor/include/Panels/AView.h"

#include "Editor/include/Core/EditorRenderer.h"
#include "Runtime/Core/App/ApplicationBase.h"
#include "Runtime/Function/UI/Widgets/Visual/Image.h"

#include "Runtime/Function/Renderer/RenderCamera.h"
#include "Runtime/Core/Log/debug.h"

#include "Runtime\Function\UI\ImGui\ImGui_TransformGizmo.h"

using namespace LitchiRuntime;
LitchiEditor::AView::AView
(
	const std::string& p_title,
	bool p_opened,
	const PanelWindowSettings& p_windowSettings,
	RendererPath* rendererPath
) :
	PanelWindow(p_title, p_opened, p_windowSettings)
{
	m_rendererPath = rendererPath;
	m_camera = m_rendererPath->GetRenderCamera();

	// ��rt��ImGui��image
	m_image = &CreateWidget<Image>(m_rendererPath->GetColorRenderTarget().get(), Vector2{ 0.f, 0.f });

	scrollable = false;
}

void LitchiEditor::AView::UpdateView(float p_deltaTime)
{
	// ����fbo�Ĵ�С
	auto [winWidth, winHeight] = GetSafeSize();

	// 

	m_image->size = Vector2(static_cast<float>(winWidth), static_cast<float>(winHeight));
	m_camera->Update();
}

void LitchiEditor::AView::_Draw_Impl()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	PanelWindow::_Draw_Impl();

	ImGui::PopStyleVar();
}

void LitchiEditor::AView::Render()
{
	_Render_Impl();
}

void LitchiEditor::AView::SetCameraPosition(const Vector3& p_position)
{
	m_cameraPosition = p_position;
}

void LitchiEditor::AView::SetCameraRotation(const Quaternion& p_rotation)
{
	m_cameraRotation = p_rotation;
}

const Vector3& LitchiEditor::AView::GetCameraPosition() const
{
	return m_cameraPosition;
}

const Quaternion& LitchiEditor::AView::GetCameraRotation() const
{
	return m_cameraRotation;
}

LitchiRuntime::RenderCamera* LitchiEditor::AView::GetCamera()
{
	return m_camera;
}

std::pair<uint16_t, uint16_t> LitchiEditor::AView::GetSafeSize() const
{
	auto result = GetSize() - Vector2{ 0.f, 25.f }; // 25 == title bar height
	return { static_cast<uint16_t>(result.x), static_cast<uint16_t>(result.y) };
}

const Vector3& LitchiEditor::AView::GetGridColor() const
{
	return m_gridColor;
}

void LitchiEditor::AView::SetGridColor(const Vector3& p_color)
{
	m_gridColor = p_color;
}

void LitchiEditor::AView::PrepareCamera()
{
	auto [winWidth, winHeight] = GetSafeSize();
	
	// m_cameraPosition = Vector3(0.0f, 0.0f, 0.0f);
	m_camera->SetViewport(winWidth, winHeight);
}
