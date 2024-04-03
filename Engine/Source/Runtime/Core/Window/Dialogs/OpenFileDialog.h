
#pragma once

#include "FileDialog.h"

namespace LitchiRuntime
{
	/**
	 * @brief Dialog window that asks the user to select a file from the disk
	 */
	class OpenFileDialog : public FileDialog
	{
	public:
		/**
		 * @brief Constructor
		 * @param p_dialogTitle 
		 */
		OpenFileDialog(const std::string& p_dialogTitle);

		/**
		 * @brief Add a supported file type to the dialog window
		 * @param p_label 
		 * @param p_filter 
		 */
		void AddFileType(const std::string& p_label, const std::string& p_filter);
	};
}