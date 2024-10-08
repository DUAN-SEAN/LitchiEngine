
#include "Runtime/Core/pch.h"
#include "Editor/include/Panels/GameView.h"

namespace LitchiEditor
{
	GameView::GameView(const std::string& p_title, bool p_opened, const PanelWindowSettings& p_windowSettings, RendererPath* rendererPath) :
		AView(p_title, p_opened, p_windowSettings, rendererPath)
	{
	}

	void GameView::UpdateView(float p_deltaTime)
	{
		AView::UpdateView(p_deltaTime);

		auto renderTargetTexture = m_rendererPath->GetColorRenderTarget().get();
		if(renderTargetTexture)
		{
			m_image->UpdateRenderTarget(renderTargetTexture);
		}
	}

	void GameView::_Render_Impl()
	{
	}

	void GameView::OnDraw()
	{
		AView::OnDraw();
	}
}
