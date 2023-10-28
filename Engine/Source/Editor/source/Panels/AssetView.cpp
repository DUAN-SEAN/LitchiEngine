//
//
//#include "Editor/include/Panels/AssetView.h"
//
//#include "Runtime/Core/Global/ServiceLocator.h"
//#include "Runtime/Function/Renderer/RenderCamera.h"
//#include "Runtime/Function/UI/Plugins/DDTarget.h"
//#include "Runtime/Function/UI/Widgets/Layout/Group.h"
//#include <Runtime/Resource/ModelManager.h>
//#include <Runtime/Resource/TextureManager.h>
//#include <Runtime/Resource/MaterialManager.h>
//
////#include "Editor/include/ApplicationEditor.h"
//#include "Runtime/Core/Tools/Utils/PathParser.h"
//
//LitchiEditor::AssetView::AssetView
//(
//	const std::string& p_title,
//	bool p_opened,
//	const PanelWindowSettings& p_windowSettings
//) : AViewControllable(p_title, p_opened, p_windowSettings)
//{
//	m_camera->SetClearColor({ 0.098f, 0.098f, 0.098f });
//	m_camera->SetFar(5000.0f);
//
//	m_resource = static_cast<Model*>(nullptr);
//	m_image->AddPlugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [this](auto p_data)
//	{
//		std::string path = p_data.first;
//
//		switch (PathParser::GetFileType(path))
//		{
//		case PathParser::EFileType::MODEL:
//			if (auto resource =ServiceLocator::Get<ModelManager>().GetResource(path); resource)
//				m_resource = resource;
//			break;
//		/*case PathParser::EFileType::TEXTURE:
//			if (auto resource =ServiceLocator::Get<TextureManager>().GetResource(path); resource)
//				m_resource = resource;
//			break;*/ // todo:
//
//		case PathParser::EFileType::MATERIAL:
//			if (auto resource =ServiceLocator::Get<MaterialManager>().GetResource(path); resource)
//				m_resource = resource;
//			break;
//		}
//	};
//}
//
//void LitchiEditor::AssetView::_Render_Impl()
//{
//	PrepareCamera();
//
//	// todo:
//
//	//auto& baseRenderer = *LitchiEditor::ApplicationEditor::Instance()->renderer;
//
//	//m_fbo.Bind();
//
//	//baseRenderer.SetStencilMask(0xFF);
//	//baseRenderer.Clear(m_camera);
//	//baseRenderer.SetStencilMask(0x00);
//
//	//uint8_t glState = baseRenderer.FetchGLState();
//	//baseRenderer.ApplyStateMask(glState);
//
//	//ApplicationEditor::Instance()->editorRenderer->RenderGrid(m_cameraPosition, m_gridColor);
//
//	//if (auto pval = std::get_if<Model*>(&m_resource); pval && *pval)
//	//	ApplicationEditor::Instance()->editorRenderer->RenderModelAsset(**pval);
//	//
//	//if (auto pval = std::get_if<Texture*>(&m_resource); pval && *pval)
//	//	ApplicationEditor::Instance()->editorRenderer->RenderTextureAsset(**pval);
//	//
//	//if (auto pval = std::get_if<Resource::Material*>(&m_resource); pval && *pval)
//	//	ApplicationEditor::Instance()->editorRenderer->RenderMaterialAsset(**pval);
//
//	//baseRenderer.ApplyStateMask(glState);
//
//	//m_fbo.Unbind();
//}
//
//void LitchiEditor::AssetView::SetResource(ViewableResource p_resource)
//{
//	m_resource = p_resource;
//}
//
//LitchiEditor::AssetView::ViewableResource LitchiEditor::AssetView::GetResource() const
//{
//	return m_resource;
//}
