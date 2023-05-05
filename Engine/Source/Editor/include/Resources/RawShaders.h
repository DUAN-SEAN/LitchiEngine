
#pragma once

#include <string>

namespace LitchiEditor
{
	/**
	* Defines some editor shaders
	*/
	class RawShaders
	{
	public:
		/**
		* Returns the grid shader
		*/
		static std::pair<std::string, std::string> GetGrid();

		/**
		* Returns the gizmo shader
		*/
		static std::pair<std::string, std::string> GetGizmo();

		/**
		* Returns the billboard shader
		*/
		static std::pair<std::string, std::string> GetBillboard();
	};
}