

#include "Editor/include/Panels/AView.h"

#include "Runtime/Function/UI/Widgets/Visual/Image.h"

LitchiRuntime::AView::AView
(
	const std::string& p_title,
	bool p_opened,
	const PanelWindowSettings& p_windowSettings
) : PanelWindow(p_title, p_opened, p_windowSettings), m_editorRenderer(EDITOR_RENDERER())
{
	m_cameraPosition = glm::vec3( - 10.0f, 3.0f, 10.0f );
	m_cameraRotation = glm::quat(glm::vec3(0.0f, 135.0f, 0.0f));
	//m_cameraRotation = glm::quat({0.0f, 135.0f, 0.0f});

	m_image = &CreateWidget<Image>(m_fbo.GetTextureID(), glm::vec2{ 0.f, 0.f });

    scrollable = false;
}

void LitchiRuntime::AView::Update(float p_deltaTime)
{
	auto[winWidth, winHeight] = GetSafeSize();

	m_image->size = glm::vec2(static_cast<float>(winWidth), static_cast<float>(winHeight));

	m_fbo.Resize(winWidth, winHeight);
}

void LitchiRuntime::AView::_Draw_Impl()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	PanelWindow::_Draw_Impl();

	ImGui::PopStyleVar();
}

void LitchiRuntime::AView::Render()
{
	FillEngineUBO();

	auto [winWidth, winHeight] = GetSafeSize();

	EDITOR_CONTEXT(shapeDrawer)->SetViewProjection(m_camera.GetProjectionMatrix() * m_camera.GetViewMatrix());

	EDITOR_CONTEXT(renderer)->SetViewPort(0, 0, winWidth, winHeight);

	_Render_Impl();
}

void LitchiRuntime::AView::SetCameraPosition(const glm::vec3 & p_position)
{
	m_cameraPosition = p_position;
}

void LitchiRuntime::AView::SetCameraRotation(const glm::quat& p_rotation)
{
	m_cameraRotation = p_rotation;
}

const glm::vec3 & LitchiRuntime::AView::GetCameraPosition() const
{
	return m_cameraPosition;
}

const glm::quat& LitchiRuntime::AView::GetCameraRotation() const
{
	return m_cameraRotation;
}

LitchiRuntime::RenderCamera& LitchiRuntime::AView::GetCamera()
{
	return *m_camera;
}

std::pair<uint16_t, uint16_t> LitchiRuntime::AView::GetSafeSize() const
{
	auto result = GetSize() - glm::vec2{ 0.f, 25.f }; // 25 == title bar height
	return { static_cast<uint16_t>(result.x), static_cast<uint16_t>(result.y) };
}

const glm::vec3& LitchiRuntime::AView::GetGridColor() const
{
	return m_gridColor;
}

void LitchiRuntime::AView::SetGridColor(const glm::vec3& p_color)
{
	m_gridColor = p_color;
}

void LitchiRuntime::AView::FillEngineUBO()
{
	auto& engineUBO = *EDITOR_CONTEXT(engineUBO);

	auto[winWidth, winHeight] = GetSafeSize();

	size_t offset = sizeof(OvMaths::FMatrix4); // We skip the model matrix (Which is a special case, modified every draw calls)
	engineUBO.SetSubData(OvMaths::FMatrix4::Transpose(m_camera.GetViewMatrix()), std::ref(offset));
	engineUBO.SetSubData(OvMaths::FMatrix4::Transpose(m_camera.GetProjectionMatrix()), std::ref(offset));
	engineUBO.SetSubData(m_cameraPosition, std::ref(offset));
}

void LitchiRuntime::AView::PrepareCamera()
{
	auto [winWidth, winHeight] = GetSafeSize();
	m_camera.CacheMatrices(winWidth, winHeight, m_cameraPosition, m_cameraRotation);
}
