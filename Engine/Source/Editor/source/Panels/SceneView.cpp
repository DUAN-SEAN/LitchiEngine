
#include "Runtime/Core/pch.h"
#include "Editor/include/Panels/SceneView.h"

#include "Editor/include/ApplicationEditor.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshRenderer.h"
#include "Runtime/Function/Renderer/RenderCamera.h"
#include "Runtime/Function/Renderer/Light/Light.h"
#include "Runtime/Function/UI/Helpers/GUIDrawer.h"
#include "Runtime/Function/UI/ImGui/ImGui_TransformGizmo.h"
#include "Runtime/Function/UI/Panels/InnerPanelWindow.h"
#include "Runtime/Function/UI/Widgets/Buttons/Button.h"
#include "Runtime/Function/UI/Widgets/Buttons/ButtonSmall.h"
#include "Runtime/Function/UI/Widgets/Layout/Columns.h"
#include "Runtime/Function/UI/Widgets/Layout/GroupCollapsable.h"
#include "Runtime/Function/UI/Widgets/Layout/TreeNode.h"
#include "Runtime/Function/UI/Widgets/Selection/CheckBox.h"
#include "Runtime/Function/UI/Widgets/Texts/Text.h"
#include "Runtime/Function/UI/Widgets/Visual/OverlapStart.h"
#include "Runtime/Function/UI/Widgets/Visual/Separator.h"

LitchiEditor::SceneView::SceneView
(
	const std::string& p_title,
	bool p_opened,
	const PanelWindowSettings& p_windowSettings,
	RendererPath* rendererPath
) : AViewControllable(p_title, p_opened, p_windowSettings, rendererPath, true)
{
	m_camera->SetClearColor({ 0.098f, 0.098f, 0.098f });

	m_transform_gizmo = &CreateWidget<TransformGizmo>(m_camera);

	CreateWidget<OverlapStart>(Vector2(0.0f, 20.0f));

	CreateCameraControlPanel();

	GameObject::DestroyedEvent += std::bind(&SceneView::DeleteActorByInstance, this, std::placeholders::_1);
}

void LitchiEditor::SceneView::UpdateView(float p_deltaTime)
{
	AViewControllable::UpdateView(p_deltaTime);
}

void LitchiEditor::SceneView::_Render_Impl()
{
	PrepareCamera();
}

void LitchiEditor::SceneView::OnDraw()
{
	// mouse picking
	if (ImGui::IsMouseClicked(0) && IsHovered() && ImGui::TransformGizmo::allow_picking())
	{
		if (auto camera = m_camera)
		{
			camera->Pick();
			auto selectedGO = camera->GetSelectedEntity();
			if(selectedGO)
			{
				ApplicationEditor::Instance()->SelectActor(selectedGO);// temp
			}
		}
	}

	AView::OnDraw();
}

void LitchiEditor::SceneView::RenderSceneForActorPicking()
{
}

bool IsResizing()
{
	auto cursor = ImGui::GetMouseCursor();

	return
		cursor == ImGuiMouseCursor_ResizeEW ||
		cursor == ImGuiMouseCursor_ResizeNS ||
		cursor == ImGuiMouseCursor_ResizeNWSE ||
		cursor == ImGuiMouseCursor_ResizeNESW ||
		cursor == ImGuiMouseCursor_ResizeAll;;
}

void LitchiEditor::SceneView::HandleActorPicking()
{
}

void LitchiEditor::SceneView::DeleteActorByInstance(GameObject* p_actor)
{
	if (auto camera = m_camera)
	{
		auto selectedGO = camera->GetSelectedEntity();
		if (selectedGO == p_actor)
		{
			camera->SetSelectedEntity(nullptr);
		}
	}
}

void LitchiEditor::SceneView::CreateCameraControlPanel()
{
	PanelWindowSettings settings;
	settings.resizable = false;
	settings.autoSize = false;
	settings.titleBar = false;
	settings.allowHorizontalScrollbar = true;
	settings.closable = false;
	settings.dockable = false;
	settings.movable = false;
	settings.scrollable = true;

	m_innerMenuWindow = &CreateWidget<InnerPanelWindow>("SceneViewMenu", true, Vector2(0.0f,20.0f), Color(0, 0, 0, 0.1f), settings);
	auto& propertyBtn = m_innerMenuWindow->CreateWidget<ButtonSmall>("Property");
	propertyBtn.ClickedEvent += [this]() {
		if (m_innerPropertyWindow->IsOpened())
		{
			m_innerPropertyWindow->Close();
		}
		else
		{
			m_innerPropertyWindow->Open();
		}
		};


	m_innerPropertyWindow = &CreateWidget<InnerPanelWindow>("SceneViewProperty",true,Vector2(300.0f,200.0f), Color(0, 0, 0, 0.1f), settings);
	auto& propertyRoot = m_innerPropertyWindow->CreateWidget<Columns<2>>();
	propertyRoot.widths[0] = 100.0;
	propertyRoot.widths[1] = 200.0f;
	GUIDrawer::DrawInputField4Float(propertyRoot, "NearPlane", [this](){return m_camera->GetNearPlane();}, [this](float value) { m_camera->SetNearPlane(value); });
	GUIDrawer::DrawInputField4Float(propertyRoot, "FarPlane", [this](){return m_camera->GetFarPlane();}, [this](float value) { m_camera->SetFarPlane(value); });
	GUIDrawer::DrawInputField4Float(propertyRoot, "FovHorizontal", [this](){return m_camera->GetFovHorizontalDeg();}, [this](float value) { m_camera->SetFovHorizontalDeg(value); });

	// Draw ProjectType
	auto getString = [this]
		{
			auto type = type::get<ProjectionType>().get_enumeration().value_to_name(m_camera->GetProjectionType());
			return type.to_string();
		};

	auto setString = [this](std::string value)
		{
			auto type = type::get<ProjectionType>().get_enumeration().name_to_value(value).convert<ProjectionType>();
			m_camera->SetProjection(type);
		};
	enumeration enum_align = type::get<ProjectionType>().get_enumeration();
	std::vector<std::string> enumValueList;
	for (auto enumName : enum_align.get_names())
	{
		enumValueList.push_back(enumName.to_string());
	}
	GUIDrawer::DrawEnum(propertyRoot, "ProjectionType", enumValueList, getString, setString, nullptr);

	auto& lightControlPanelRoot = m_innerPropertyWindow->CreateWidget<Group>();
	lightControlPanelRoot.CreateWidget<Text>("LightControlPanel");
	auto resetButton = lightControlPanelRoot.CreateWidget<Button>("SceneView Button");


}

