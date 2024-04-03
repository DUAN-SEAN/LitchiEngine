
#pragma once

#include "FileDialog.h"

namespace LitchiRuntime
{
	/**
	 * @brief  Dialog window that asks the user to save a file to the disk
	 */
	class SaveFileDialog : public FileDialog
	{
	public:
		/**
		 * @brief Constructor
		 * @param p_dialogTitle 
		 */
		SaveFileDialog(const std::string& p_dialogTitle);

		/**
		 * @brief Show the file dialog
		 * @param p_flags 
		 */
		virtual void Show(EExplorerFlags p_flags = EExplorerFlags::DONTADDTORECENT | EExplorerFlags::FILEMUSTEXIST | EExplorerFlags::HIDEREADONLY | EExplorerFlags::NOCHANGEDIR) override;

		/**
		 * @brief Define the extension of the saved file
		 * @param p_label 
		 * @param p_extension 
		 */
		void DefineExtension(const std::string& p_label, const std::string& p_extension);

	private:
		void AddExtensionToFilePathAndName();

	private:
		std::string m_extension;
	};
}