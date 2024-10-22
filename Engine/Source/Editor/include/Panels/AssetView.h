
#pragma once

#include <variant>
#include "AViewControllable.h"

namespace LitchiEditor
{
	/**
	* Provide a view for assets
	*/
	class AssetView : public LitchiEditor::AViewControllable
	{
	public:
		using ViewableResource = std::variant<Mesh*, RHI_Texture2D*, Material*>;

		/**
		* Constructor
		* @param p_title
		* @param p_opened
		* @param p_windowSettings
		*/
		AssetView
		(
			const std::string& p_title,
			bool p_opened,
			const PanelWindowSettings& p_windowSettings, RendererPath* rendererPath
		);

		/**
		* Custom implementation of the render method
		*/
		virtual void _Render_Impl() override;

		/**
		* Defines the resource to preview
		* @parma p_resource
		*/
		void SetResource(ViewableResource p_resource);

		/**
		* Return the currently previewed resource
		*/
		ViewableResource GetResource() const;
		
	private:
		ViewableResource m_resource;
	};
}
