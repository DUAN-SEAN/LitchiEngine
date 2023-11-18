
#include "Editor/include/Panels/MaterialEditor.h"

#include <string>

#include "Editor/include/Core/EditorActions.h"
#include "Editor/include/Panels/AssetView.h"
#include "Runtime/Function/UI/Helpers/GUIDrawer.h"
#include "Runtime/Function/UI/Widgets/Buttons/Button.h"
#include "Runtime/Function/UI/Widgets/Drags/DragMultipleScalars.h"
#include "Runtime/Function/UI/Widgets/Layout/Group.h"
#include "Runtime/Function/UI/Widgets/Selection/ColorEdit.h"
#include "Runtime/Function/UI/Widgets/Visual/Separator.h"
#include "Runtime/Core/Global/ServiceLocator.h"
#include "Runtime/Function/Renderer/Resources/Loaders/MaterialLoader.h"
#include "Runtime/Function/UI/Widgets/Layout/GroupCollapsable.h"
#include "Runtime/Resource/MaterialManager.h"


void DrawHybridVec3(WidgetContainer& p_root, const std::string& p_name, Vector3& p_data, float p_step, float p_min, float p_max)
{
	GUIDrawer::CreateTitle(p_root, p_name);

	auto& rightSide = p_root.CreateWidget<LitchiRuntime::Group>();

	auto& xyzWidget = rightSide.CreateWidget<DragMultipleScalars<float, 3>>(GUIDrawer::GetDataType<float>(), p_min, p_max, 0.f, p_step, "", GUIDrawer::GetFormat<float>());
	auto& xyzDispatcher = xyzWidget.AddPlugin<DataDispatcher<std::array<float, 3>>>();
	xyzDispatcher.RegisterReference(reinterpret_cast<std::array<float, 3>&>(p_data));
	xyzWidget.lineBreak = false;

	auto& rgbWidget = rightSide.CreateWidget<ColorEdit>(false, Color{ p_data.x, p_data.y, p_data.z });
	auto& rgbDispatcher = rgbWidget.AddPlugin<DataDispatcher<Color>>();
	rgbDispatcher.RegisterReference(reinterpret_cast<Color&>(p_data));
	rgbWidget.enabled = false;
	rgbWidget.lineBreak = false;

	auto& xyzButton = rightSide.CreateWidget<Button>("XYZ");
	xyzButton.idleBackgroundColor = { 0.7f, 0.5f, 0.0f };
	xyzButton.lineBreak = false;

	auto& rgbButton = rightSide.CreateWidget<Button>("RGB");
	rgbButton.idleBackgroundColor = { 0.7f, 0.5f, 0.0f };

	xyzButton.ClickedEvent += [&]
	{
		xyzWidget.enabled = true;
		rgbWidget.enabled = false;
	};

	rgbButton.ClickedEvent += [&]
	{
		xyzWidget.enabled = false;
		rgbWidget.enabled = true;
	};
}

void DrawHybridVec4(WidgetContainer& p_root, const std::string& p_name, Vector4& p_data, float p_step, float p_min, float p_max)
{
	GUIDrawer::CreateTitle(p_root, p_name);

	auto& rightSide = p_root.CreateWidget<Group>();

	auto& xyzWidget = rightSide.CreateWidget<DragMultipleScalars<float, 4>>(GUIDrawer::GetDataType<float>(), p_min, p_max, 0.f, p_step, "", GUIDrawer::GetFormat<float>());
	auto& xyzDispatcher = xyzWidget.AddPlugin<DataDispatcher<std::array<float, 4>>>();
	xyzDispatcher.RegisterReference(reinterpret_cast<std::array<float, 4>&>(p_data));
	xyzWidget.lineBreak = false;

	auto& rgbaWidget = rightSide.CreateWidget<ColorEdit>(true, Color{ p_data.x, p_data.y, p_data.z, p_data.w });
	auto& rgbaDispatcher = rgbaWidget.AddPlugin<DataDispatcher<Color>>();
	rgbaDispatcher.RegisterReference(reinterpret_cast<Color&>(p_data));
	rgbaWidget.enabled = false;
	rgbaWidget.lineBreak = false;

	auto& xyzwButton = rightSide.CreateWidget<Button>("XYZW");
	xyzwButton.idleBackgroundColor = { 0.7f, 0.5f, 0.0f };
	xyzwButton.lineBreak = false;

	auto& rgbaButton = rightSide.CreateWidget<Button>("RGBA");
	rgbaButton.idleBackgroundColor = { 0.7f, 0.5f, 0.0f };

	xyzwButton.ClickedEvent += [&]
	{
		xyzWidget.enabled = true;
		rgbaWidget.enabled = false;
	};

	rgbaButton.ClickedEvent += [&]
	{
		xyzWidget.enabled = false;
		rgbaWidget.enabled = true;
	};
}

LitchiEditor::MaterialEditor::MaterialEditor
(
	const std::string& p_title,
	bool p_opened,
	const PanelWindowSettings& p_windowSettings
) :
	PanelWindow(p_title, p_opened, p_windowSettings)
{
	CreateHeaderButtons();
	CreateWidget<Separator>();
	CreateMaterialSelector();
	m_settings = &CreateWidget<Group>();
	CreateShaderSelector();
	CreateMaterialSettings();
	CreateShaderSettings();

	m_settings->enabled = false;
	m_shaderSettings->enabled = false;

	m_materialDroppedEvent	+= std::bind(&MaterialEditor::OnMaterialDropped, this);
	m_shaderDroppedEvent	+= std::bind(&MaterialEditor::OnShaderDropped, this);
}

void LitchiEditor::MaterialEditor::Refresh()
{
	if (m_target)
		SetTarget(m_target);
}

void LitchiEditor::MaterialEditor::SetTarget(Material* p_newTarget)
{
	m_target = p_newTarget;
	m_targetMaterialText->content = m_target->GetResourceFilePath();
	OnMaterialDropped();
}

Material* LitchiEditor::MaterialEditor::GetTarget() const
{
	return m_target;
}

void LitchiEditor::MaterialEditor::RemoveTarget()
{
	m_target = nullptr;
	m_targetMaterialText->content = "Empty";
	OnMaterialDropped();
}

void LitchiEditor::MaterialEditor::Preview()
{
	// todo: 

	//auto& assetView = EDITOR_PANEL(AssetView, "Asset View");

	//if (m_target)
	//	assetView.SetResource(m_target);

	//assetView.Open();
}

void LitchiEditor::MaterialEditor::Reset()
{
	if (m_target && m_shader)
	{
		m_target->SetShader(nullptr);
		OnShaderDropped();
	}
}

void LitchiEditor::MaterialEditor::OnMaterialDropped()
{
	m_settings->enabled = m_target; // Enable m_settings group if the target material is non-null

	if (m_settings->enabled)
	{
		GenerateMaterialSettingsContent();
		m_shaderText->content = m_target->GetShader() ? m_target->GetShader()->m_shaderPath : "Empty";
		m_shader = m_target->GetShader();
	}
	else
	{
		m_materialSettingsColumns->RemoveAllWidgets();
	}

	m_shaderSettings->enabled = false;
	m_shaderSettingsColumns->RemoveAllWidgets();

	if (m_target && m_target->GetShader())
		OnShaderDropped();
}

void LitchiEditor::MaterialEditor::OnShaderDropped()
{
	m_shaderSettings->enabled = m_shader; // Enable m_shaderSettings group if the shader of the target material is non-null

	if (m_shader != m_target->GetShader())
		m_target->SetShader(m_shader);

	if (m_shaderSettings->enabled)
	{
		GenerateShaderSettingsContent();
	}
	else
	{
		m_shaderSettingsColumns->RemoveAllWidgets();
	}
}

void LitchiEditor::MaterialEditor::CreateHeaderButtons()
{
	auto& saveButton = CreateWidget<Button>("Save to file");
	saveButton.idleBackgroundColor = { 0.0f, 0.5f, 0.0f };
	saveButton.ClickedEvent += [this]
	{
		if (m_target)
		{
			m_target->SaveToFile(m_target->GetResourceFilePath());
		}
		// Loaders::MaterialLoader::Save(*m_target, EDITOR_EXEC(GetRealPath(m_target->path)));
	};

	saveButton.lineBreak = false;

	auto& reloadButton = CreateWidget<Button>("Reload from file");
	reloadButton.idleBackgroundColor = { 0.7f, 0.5f, 0.0f };
	reloadButton.ClickedEvent += [this]
	{
		if (m_target)
		{
			m_target->LoadFromFile(m_target->GetResourceFilePath());
		}
			// Loaders::MaterialLoader::Reload(*m_target, EDITOR_EXEC(GetRealPath(m_target->path)));

		OnMaterialDropped();
	};

	reloadButton.lineBreak = false;

	auto& previewButton = CreateWidget<Button>("Preview");
	previewButton.idleBackgroundColor = { 0.7f, 0.5f, 0.0f };
	previewButton.ClickedEvent += std::bind(&MaterialEditor::Preview, this);
	previewButton.lineBreak = false;

	auto& resetButton = CreateWidget<Button>("Reset to default");
	resetButton.idleBackgroundColor = { 0.5f, 0.0f, 0.0f };
	resetButton.ClickedEvent += std::bind(&MaterialEditor::Reset, this);
}

void LitchiEditor::MaterialEditor::CreateMaterialSelector()
{
	auto& columns = CreateWidget<Columns<2>>();
	columns.widths[0] = 150;
	m_targetMaterialText = &GUIDrawer::DrawMaterial(columns, "Material", m_target, &m_materialDroppedEvent);
}

void LitchiEditor::MaterialEditor::CreateShaderSelector()
{
	auto& columns = m_settings->CreateWidget<Columns<2>>();
	columns.widths[0] = 150;
	m_shaderText = &GUIDrawer::DrawShader(columns, "Shader", m_shader, &m_shaderDroppedEvent);
}

void LitchiEditor::MaterialEditor::CreateMaterialSettings()
{
	m_materialSettings = &m_settings->CreateWidget<GroupCollapsable>("Material Settings");
	m_materialSettingsColumns = &m_materialSettings->CreateWidget<Columns<2>>();
	m_materialSettingsColumns->widths[0] = 150;
}

void LitchiEditor::MaterialEditor::CreateShaderSettings()
{
	m_shaderSettings = &m_settings->CreateWidget<GroupCollapsable>("Shader Settings");
	m_shaderSettingsColumns = &m_shaderSettings->CreateWidget<Columns<2>>();
	m_shaderSettingsColumns->widths[0] = 150;
}

std::string UniformFormat(const std::string& p_string)
{
	std::string result;
	std::string formattedInput = p_string;

	if (formattedInput.rfind("u_", 0) == 0 || formattedInput.rfind("U_", 0) == 0)
	{
		formattedInput = formattedInput.substr(2);
	}

	std::string capsChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	
	bool first = true;
	bool capsNext = false;

	for (char c : formattedInput)
	{
		if (first || capsNext)
		{
			c = toupper(c);
			first = false;
			capsNext = false;
		}

		if (c == '_')
		{
			c = ' ';
			capsNext = true;
		}

		if (std::find(capsChars.begin(), capsChars.end(), c) != capsChars.end())
			result.push_back(' ');

		result.push_back(c);
	}

	return result;
}

void LitchiEditor::MaterialEditor::GenerateShaderSettingsContent()
{
	m_shaderSettingsColumns->RemoveAllWidgets(); // Ensure that the m_shaderSettingsColumns is empty

	std::multimap<int, std::pair<std::string, std::any*>> sortedUniformsData;

	for (auto&[name, value] : m_target->GetUniformsData())
	{
		int orderID = 0;

		auto uniformData = m_target->GetShader()->GetGlobalUniformInfo(name);

		if (uniformData.type != UniformType::UNIFORM_Unknown)
		{
			switch (uniformData.type)
			{
			case UniformType::UNIFORM_TEXTURE:	orderID = 0; break;
			case UniformType::UNIFORM_FLOAT_VEC4:	orderID = 1; break;
			case UniformType::UNIFORM_FLOAT_VEC3:	orderID = 2; break;
			case UniformType::UNIFORM_FLOAT_VEC2:	orderID = 3; break;
			case UniformType::UNIFORM_FLOAT:		orderID = 4; break;
			case UniformType::UNIFORM_INT:			orderID = 5; break;
			case UniformType::UNIFORM_BOOL:			orderID = 6; break;
			}

			sortedUniformsData.emplace(orderID, std::pair<std::string, std::any*>{ name, & value });
		}else
		{

			auto textureDesc = m_target->GetShader()->GetTextureDescriptor(name);
			if(textureDesc.type == RHI_Descriptor_Type::Texture)
			{
				orderID = 6;
				sortedUniformsData.emplace(orderID, std::pair<std::string, std::any*>{ name, & value });
			}
			
		}

	}

	for (auto& [order, info] : sortedUniformsData)
	{
		auto uniformData = m_target->GetShader()->GetGlobalUniformInfo(info.first);
		
		if (uniformData.type != UniformType::UNIFORM_Unknown)
		{
			switch (uniformData.type)
			{
			case UniformType::UNIFORM_BOOL:			GUIDrawer::DrawBoolean(*m_shaderSettingsColumns, UniformFormat(info.first), reinterpret_cast<bool&>(*info.second));																	break;
			case UniformType::UNIFORM_INT:			GUIDrawer::DrawScalar<int>(*m_shaderSettingsColumns, UniformFormat(info.first), reinterpret_cast<int&>(*info.second));																break;
			case UniformType::UNIFORM_FLOAT:		GUIDrawer::DrawScalar<float>(*m_shaderSettingsColumns, UniformFormat(info.first), reinterpret_cast<float&>(*info.second), 0.01f, GUIDrawer::_MIN_FLOAT, GUIDrawer::_MAX_FLOAT);		break;
			case UniformType::UNIFORM_FLOAT_VEC2:	GUIDrawer::DrawVec2(*m_shaderSettingsColumns, UniformFormat(info.first), reinterpret_cast<Vector2&>(*info.second), 0.01f, GUIDrawer::_MIN_FLOAT, GUIDrawer::_MAX_FLOAT);	break;
			case UniformType::UNIFORM_FLOAT_VEC3:	DrawHybridVec3(*m_shaderSettingsColumns, UniformFormat(info.first), reinterpret_cast<Vector3&>(*info.second), 0.01f, GUIDrawer::_MIN_FLOAT, GUIDrawer::_MAX_FLOAT);			break;
			case UniformType::UNIFORM_FLOAT_VEC4:	DrawHybridVec4(*m_shaderSettingsColumns, UniformFormat(info.first), reinterpret_cast<Vector4&>(*info.second), 0.01f, GUIDrawer::_MIN_FLOAT, GUIDrawer::_MAX_FLOAT);			break;
			case UniformType::UNIFORM_TEXTURE:	GUIDrawer::DrawTexture(*m_shaderSettingsColumns, UniformFormat(info.first), reinterpret_cast<RHI_Texture*&>(*info.second));																break;
			}
		}else
		{
			auto textureDesc = m_target->GetShader()->GetTextureDescriptor(info.first);
			if (textureDesc.type == RHI_Descriptor_Type::Texture)
			{
				GUIDrawer::DrawTexture(*m_shaderSettingsColumns, UniformFormat(info.first), reinterpret_cast<RHI_Texture*&>(*info.second));
			}
		}
	}



}

void LitchiEditor::MaterialEditor::GenerateMaterialSettingsContent()
{
	m_materialSettingsColumns->RemoveAllWidgets(); // Ensure that the m_shaderSettingsColumns is empty

	/*GUIDrawer::DrawBoolean(*m_materialSettingsColumns, "Blendable", std::bind(&Resource::Material::IsBlendable, m_target), std::bind(&Resource::Material::SetBlendable, m_target, std::placeholders::_1));
	GUIDrawer::DrawBoolean(*m_materialSettingsColumns, "Back-face Culling", std::bind(&Resource::Material::HasBackfaceCulling, m_target), std::bind(&Resource::Material::SetBackfaceCulling, m_target, std::placeholders::_1));
	GUIDrawer::DrawBoolean(*m_materialSettingsColumns, "Front-face Culling", std::bind(&Resource::Material::HasFrontfaceCulling, m_target), std::bind(&Resource::Material::SetFrontfaceCulling, m_target, std::placeholders::_1));
	GUIDrawer::DrawBoolean(*m_materialSettingsColumns, "Depth Test", std::bind(&Resource::Material::HasDepthTest, m_target), std::bind(&Resource::Material::SetDepthTest, m_target, std::placeholders::_1));
	GUIDrawer::DrawBoolean(*m_materialSettingsColumns, "Depth Writing", std::bind(&Resource::Material::HasDepthWriting, m_target), std::bind(&Resource::Material::SetDepthWriting, m_target, std::placeholders::_1));
	GUIDrawer::DrawBoolean(*m_materialSettingsColumns, "Color Writing", std::bind(&Resource::Material::HasColorWriting, m_target), std::bind(&Resource::Material::SetColorWriting, m_target, std::placeholders::_1));
	GUIDrawer::DrawScalar<int>(*m_materialSettingsColumns, "GPU Instances", std::bind(&Resource::Material::GetGPUInstances, m_target), std::bind(&Resource::Material::SetGPUInstances, m_target, std::placeholders::_1), 1.0f, 0, 100000);*/
}
