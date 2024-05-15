
#include <Runtime/Function/UI/Widgets/Texts/Text.h>
#include <Runtime/Function/UI/Panels/PanelWindow.h>

#include <Runtime/Core/Tools//Filesystem/IniFile.h>

namespace LitchiEditor
{
	class ProjectSettings : public LitchiRuntime::PanelWindow
	{
	public:
		/**
		* Constructor
		* @param p_title
		* @param p_opened
		* @param p_windowSettings
		*/
		ProjectSettings
		(
			const std::string& p_title,
			bool p_opened,
			const LitchiRuntime::PanelWindowSettings& p_windowSettings
		);

		/**
		* Generate a gatherer that will get the value associated to the given key
		* @param p_keyName
		*/
		template <typename T>
		std::function<T()> GenerateGatherer(const std::string& p_keyName)
		{
			return std::bind(&LitchiRuntime::IniFile::Get<T>, &m_projectFile, p_keyName);
		}

		/**
		* Generate a provider that will set the value associated to the given key
		* @param p_keyName
		*/
		template <typename T>
		std::function<void(T)> GenerateProvider(const std::string& p_keyName)
		{
			return std::bind(&LitchiRuntime::IniFile::Set<T>, &m_projectFile, p_keyName, std::placeholders::_1);
		}

	private:
		LitchiRuntime::IniFile& m_projectFile;
	};
}