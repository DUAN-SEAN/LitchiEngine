
#pragma once

#include "Runtime/Function/Renderer/Resources/Material.h"
#include "Runtime/Function/UI/Panels/PanelWindow.h"
#include "Runtime/Function/UI/Widgets/Layout/Columns.h"
#include "Runtime/Function/UI/Widgets/Layout/Group.h"
#include "Runtime/Function/UI/Widgets/Texts/Text.h"

using namespace LitchiRuntime;
namespace LitchiEditor
{
	class MaterialEditor : public LitchiRuntime::PanelWindow
	{
	public:
		/**
		* Constructor
		* @param p_title
		* @param p_opened
		* @param p_windowSettings
		*/
		MaterialEditor
		(
			const std::string& p_title,
			bool p_opened,
			const LitchiRuntime::PanelWindowSettings& p_windowSettings
		);

		/**
		* Refresh the material editor
		*/
		void Refresh();

		/**
		* Defines the target material of the material editor
		* @param p_newTarget
		*/
		void SetTarget(Resource::Material& p_newTarget);

		/**
		* Returns the target of the material editor
		*/
		Resource::Material* GetTarget() const;

		/**
		* Remove the target of the material editor (Clear the material editor)
		*/
		void RemoveTarget();

		/**
		* Launch the preview of the currently targeted material
		*/
		void Preview();

		/**
		* Reset material
		*/
		void Reset();
		
	private:
		void OnMaterialDropped();
		void OnShaderDropped();

		void CreateHeaderButtons();
		void CreateMaterialSelector();
		void CreateShaderSelector();
		void CreateMaterialSettings();
		void CreateShaderSettings();

		void GenerateShaderSettingsContent();
		void GenerateMaterialSettingsContent();

	private:
		Resource::Material* m_target		= nullptr;
		Resource::Shader* m_shader	= nullptr;

		Text* m_targetMaterialText	= nullptr;
		Text* m_shaderText			= nullptr;

		Event<> m_materialDroppedEvent;
		Event<> m_shaderDroppedEvent;

		Group* m_settings			= nullptr;
		Group* m_materialSettings	= nullptr;
		Group* m_shaderSettings		= nullptr;

		Columns<2>* m_shaderSettingsColumns = nullptr;
		Columns<2>* m_materialSettingsColumns = nullptr;
	};
}