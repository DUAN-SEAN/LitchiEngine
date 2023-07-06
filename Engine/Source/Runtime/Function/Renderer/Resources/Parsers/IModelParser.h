
#pragma once

#include <string>

#include "Runtime/Function/Renderer/Resources/Mesh.h"
#include <Runtime/Function/Renderer/Resources/Model.h>

namespace LitchiRuntime::Parsers
{
	/**
	* Interface for any model parser
	*/
	class IModelParser
	{
	public:
		/**
		* Load meshes from a file
		* Return true on success
		* @param p_filename
		* @param p_meshes
		* @param p_parserFlags
		*/
		virtual bool LoadModel
		(
			const std::string& p_fileName,
			Model* model,
			EModelParserFlags p_parserFlags
		) = 0;
	};
}
