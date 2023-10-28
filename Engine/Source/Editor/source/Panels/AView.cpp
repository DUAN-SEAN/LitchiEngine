

#include "Editor/include/Panels/AView.h"

#include <glad/glad.h>

#include "Editor/include/ApplicationEditor.h"
#include "Editor/include/Core/EditorRenderer.h"
#include "Runtime/Function/UI/Widgets/Visual/Image.h"

#include "Runtime/Function/Renderer/RenderCamera.h"
#include "Runtime/Core/Log/debug.h"

using namespace LitchiRuntime;
LitchiEditor::AView::AView
(
	const std::string& p_title,
	bool p_opened,
	const PanelWindowSettings& p_windowSettings
) :
	PanelWindow(p_title, p_opened, p_windowSettings)
{
	// 初始化Cameraf
	m_camera = new RenderCamera();

	// 绑定fbo到ImGui的image todo:
	// m_image = &CreateWidget<Image>(m_fbo.GetTextureID(), glm::vec2{ 0.f, 0.f });

	scrollable = false;
}

void LitchiEditor::AView::Update(float p_deltaTime)
{
	// 渲染绘制前更新

	// 更新fbo的大小
	auto [winWidth, winHeight] = GetSafeSize();

	m_image->size = glm::vec2(static_cast<float>(winWidth), static_cast<float>(winHeight));

	// m_fbo.Resize(winWidth, winHeight);
}

void LitchiEditor::AView::_Draw_Impl()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	PanelWindow::_Draw_Impl();

	ImGui::PopStyleVar();
}

void LitchiEditor::AView::Render()
{
	// 渲染绘制前调用
	// 1. 设置UBO
	// 2. 设置视口大小
	// 3. 执行绘制

	FillEngineUBO();

	auto [winWidth, winHeight] = GetSafeSize();

	// todo
	// ApplicationEditor::Instance()->shapeDrawer->SetViewProjection(m_camera->GetProjectionMatrix()*m_camera->GetViewMatrix());

	glViewport(0, 0, winWidth, winHeight);

	_Render_Impl();
}

void LitchiEditor::AView::SetCameraPosition(const glm::vec3& p_position)
{
	m_cameraPosition = p_position;
}

void LitchiEditor::AView::SetCameraRotation(const glm::quat& p_rotation)
{
	m_cameraRotation = p_rotation;
}

const glm::vec3& LitchiEditor::AView::GetCameraPosition() const
{
	return m_cameraPosition;
}

const glm::quat& LitchiEditor::AView::GetCameraRotation() const
{
	return m_cameraRotation;
}

LitchiRuntime::RenderCamera* LitchiEditor::AView::GetCamera()
{
	return m_camera;
}

std::pair<uint16_t, uint16_t> LitchiEditor::AView::GetSafeSize() const
{
	auto result = GetSize() - glm::vec2{ 0.f, 25.f }; // 25 == title bar height
	return { static_cast<uint16_t>(result.x), static_cast<uint16_t>(result.y) };
}

const glm::vec3& LitchiEditor::AView::GetGridColor() const
{
	return m_gridColor;
}

void LitchiEditor::AView::SetGridColor(const glm::vec3& p_color)
{
	m_gridColor = p_color;
}

void LitchiEditor::AView::FillEngineUBO()
{
	auto& engineUBO = *ApplicationEditor::Instance()->engineUBO.get();
	size_t offset = sizeof(glm::mat4); // We skip the model matrix (Which is a special case, modified every draw calls)
	engineUBO.SetSubData(m_camera->GetViewMatrix(), std::ref(offset));
	engineUBO.SetSubData(m_camera->GetProjectionMatrix(), std::ref(offset));
	engineUBO.SetSubData(m_cameraPosition, std::ref(offset));
}

void LitchiEditor::AView::PrepareCamera()
{
	auto [winWidth, winHeight] = GetSafeSize();
	
	// m_cameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	m_camera->CacheMatrices(winWidth, winHeight, m_cameraPosition, m_cameraRotation);
}
