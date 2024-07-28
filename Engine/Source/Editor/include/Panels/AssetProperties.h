
#pragma once

#include <variant>

#include "Runtime/Function/Renderer/Rendering/Mesh.h"
#include "Runtime/Function/UI/Panels/PanelWindow.h"
#include "Runtime/Function/UI/Widgets/Buttons/Button.h"
#include "Runtime/Function/UI/Widgets/Layout/Columns.h"
#include "Runtime/Function/UI/Widgets/Layout/Group.h"
#include "Runtime/Function/UI/Widgets/Texts/Text.h"

namespace LitchiEditor
{
	class AssetProperties : public LitchiRuntime::PanelWindow
	{
	public:
		using EditableAssets = std::variant<LitchiRuntime::Mesh*, LitchiRuntime::RHI_Texture*>;

		/**
		* Constructor
		* @param p_title
		* @param p_opened
		* @param p_windowSettings
		*/
		AssetProperties
		(
			const std::string& p_title,
			bool p_opened,
			const LitchiRuntime::PanelWindowSettings& p_windowSettings
		);

		/**
		* Defines the target of the asset settings editor
		* @param p_path
		*/
		void SetTarget(const std::string& p_path);

        /**
        * Refresh the panel to show the current target settings
        */
        void Refresh();

		/**
		* Launch the preview of the target asset
		*/
		void Preview();

	private:
		void CreateHeaderButtons();
        void CreateAssetSelector();
		void CreateSettings();
		void CreateInfo();
		void CreateModelSettings();
		void CreateTextureSettings();
		void Apply();

	private:
		std::string m_resource;

		LitchiRuntime::Event<> m_targetChanged;
		LitchiRuntime::Group* m_settings = nullptr;
		LitchiRuntime::Group* m_info = nullptr;
		LitchiRuntime::Button* m_applyButton = nullptr;
        LitchiRuntime::Button* m_revertButton = nullptr;
        LitchiRuntime::Button* m_previewButton = nullptr;
        LitchiRuntime::Button* m_resetButton = nullptr;
		LitchiRuntime::AWidget* m_headerSeparator = nullptr;
		LitchiRuntime::AWidget* m_headerLineBreak = nullptr;
		LitchiRuntime::Columns<2>* m_settingsColumns = nullptr;
		LitchiRuntime::Columns<2>* m_infoColumns = nullptr;
		LitchiRuntime::Text* m_assetSelector = nullptr;
		std::unique_ptr<LitchiRuntime::IniFile> m_metadata;
	};
}
