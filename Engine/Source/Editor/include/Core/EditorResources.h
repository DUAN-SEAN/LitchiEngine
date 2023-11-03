//
//#pragma once
//#include <string>
//
//#include "Runtime/Function/Renderer/Resources/Model.h"
//#include "Runtime/Function/Renderer/Resources/Texture.h"
//#include "Runtime/Function/Renderer/Resources/Shader.h"
//
//
//namespace LitchiEditor
//{
//	/**
//	* Handle the creation and storage of editor specific resources
//	*/
//	class EditorResources
//	{
//	public:
//		/**
//		* Constructor
//		* @param p_editorAssetsPath
//		*/
//		EditorResources(const std::string& p_editorAssetsPath);
//
//		/**
//		* Destructor
//		*/
//		~EditorResources();
//
//		/**
//		* Returns the file icon identified by the given string or nullptr on fail
//		* @param p_filename
//		*/
//		LitchiRuntime::Texture* GetFileIcon(const std::string& p_filename);
//
//		/**
//		* Returns the texture identified by the given string or nullptr on fail
//		* @param p_id
//		*/
//		LitchiRuntime::Texture* GetTexture(const std::string& p_id);
//
//		/**
//		* Returns the model identified by the given string or nullptr on fail
//		* @param p_id
//		*/
//		LitchiRuntime::Model* GetModel(const std::string& p_id);
//
//		/**
//		* Returns the shader identified by the given string or nullptr on fail
//		* @param p_id
//		*/
//		LitchiRuntime::Shader* GetShader(const std::string& p_id);
//
//	private:
//		std::unordered_map<std::string, LitchiRuntime::Texture*> m_textures;
//		std::unordered_map<std::string, LitchiRuntime::Model*> m_models;
//		std::unordered_map<std::string, LitchiRuntime::Shader*> m_shaders;
//	};
//}
