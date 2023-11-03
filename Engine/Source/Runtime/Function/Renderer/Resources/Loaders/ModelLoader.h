//
//#pragma once
//
//#include <string>
//
//#include "Runtime/Function/Renderer/Resources/Parsers/AssimpParser.h"
//#include "Runtime/Function/Renderer/Resources/Parsers/EModelParserFlags.h"
//
//namespace LitchiRuntime
//{
//	class Model;
//}
//
//namespace LitchiRuntime::Loaders
//{
//	/**
//	* Handle the Model creation and destruction
//	*/
//	class ModelLoader
//	{
//	public:
//		/**
//		* Disabled constructor
//		*/
//		ModelLoader() = delete;
//
//		/**
//		* Create a model
//		* @param p_filepath
//		* @param p_parserFlags
//		*/
//		static Model* Create(const std::string& p_filepath, Parsers::EModelParserFlags p_parserFlags = Parsers::EModelParserFlags::NONE);
//
//		/**
//		* Reload a model from file
//		* @param p_model
//		* @param p_filePath
//		* @param p_parserFlags
//		*/
//		static void Reload(Model& p_model, const std::string& p_filePath, Parsers::EModelParserFlags p_parserFlags = Parsers::EModelParserFlags::NONE);
//
//		/**
//		* Disabled constructor
//		* @param p_modelInstance
//		*/
//		static bool Destroy(Model*& p_modelInstance);
//
//	private:
//		static Parsers::AssimpParser __ASSIMP;
//	};
//}
