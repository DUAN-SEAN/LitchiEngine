
#pragma once

#include <variant>
#include "AViewControllable.h"
#include "Runtime/Function/Renderer/Resources/Material.h"
#include "Runtime/Function/Renderer/Resources/Model.h"
#include "Runtime/Function/Renderer/Resources/Texture.h"

namespace LitchiEditor
{
	/**
	* Provide a view for assets
	*/
	class AssetView : public LitchiEditor::AViewControllable
	{
	public:
		using ViewableResource = std::variant<Model*, Texture*, Resource::Material*>;

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
			const PanelWindowSettings& p_windowSettings
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
		void RenderGrid(const glm::vec3& p_viewPos, const glm::vec3& p_color);
		void RenderModelAsset(Model& model);
		void RenderTextureAsset(Texture& texture);
		void RenderMaterialAsset(Resource::Material& material);
	private:
		ViewableResource m_resource;
	};
}
