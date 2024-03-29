
#include "Runtime/Core/pch.h"

#include "Editor/include/Panels/AssetView.h"

#include "Runtime/Core/Global/ServiceLocator.h"
#include "Runtime/Function/Renderer/RenderCamera.h"
#include "Runtime/Function/UI/Plugins/DDTarget.h"
#include "Runtime/Function/UI/Widgets/Layout/Group.h"
#include <Runtime/Resource/ModelManager.h>
#include <Runtime/Resource/TextureManager.h>
#include <Runtime/Resource/MaterialManager.h>

//#include "Editor/include/ApplicationEditor.h"
#include "Editor/include/ApplicationEditor.h"
#include "Runtime/Core/Tools/Utils/PathParser.h"

LitchiEditor::AssetView::AssetView
(
	const std::string& p_title,
	bool p_opened,
	const PanelWindowSettings& p_windowSettings, RendererPath* rendererPath
) : AViewControllable(p_title, p_opened, p_windowSettings, rendererPath)
{
	m_camera->SetClearColor({ 0.098f, 0.098f, 0.098f });

	// m_resource = static_cast<ViewableResource*>(nullptr);
	m_image->AddPlugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [this](auto p_data)
	{
		std::string path = p_data.first;

		switch (PathParser::GetFileType(path))
		{
		case PathParser::EFileType::MODEL:
			if (auto resource =ServiceLocator::Get<ModelManager>().GetResource(path); resource)
				m_resource = resource;
			break;
		case PathParser::EFileType::TEXTURE:
			if (auto resource =ServiceLocator::Get<TextureManager>().GetResource(path); resource)
				m_resource = resource;
			break;
		case PathParser::EFileType::MATERIAL:
			if (auto resource =ServiceLocator::Get<MaterialManager>().GetResource(path); resource)
				m_resource = resource;
			break;
		}
	};
}

void LitchiEditor::AssetView::_Render_Impl()
{
	auto rendererPath = ApplicationEditor::Instance()->m_rendererPath4AssetView;

	PrepareCamera();

	if (auto pval = std::get_if<Mesh*>(&m_resource); pval && *pval)
		rendererPath->SetSelectedMesh(*pval);
	
	if (auto pval = std::get_if<RHI_Texture2D*>(&m_resource); pval && *pval)
		rendererPath->SetSelectedTexture2D(*pval);
	
	if (auto pval = std::get_if<Material*>(&m_resource); pval && *pval)
		rendererPath->SetSelectedMaterial(*pval);

	rendererPath->SetActive(IsOpened());
}

void LitchiEditor::AssetView::SetResource(ViewableResource p_resource)
{
	m_resource = p_resource;
}

LitchiEditor::AssetView::ViewableResource LitchiEditor::AssetView::GetResource() const
{
	return m_resource;
}
