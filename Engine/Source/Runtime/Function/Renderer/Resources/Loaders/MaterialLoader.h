//
//#pragma once
//
//#include "Runtime/Function/Renderer/Resources/Material.h"
//
//namespace LitchiRuntime::Loaders
//{
//	/**
//	* Handle the creation of materials
//	*/
//	class MaterialLoader
//	{
//	public:
//		/**
//		* Disabled constructor
//		*/
//		MaterialLoader() = delete;
//
//		/**
//		* Instantiate a material from a file
//		* @param p_path
//		*/
//		static LitchiRuntime::Material* Create(const std::string & p_path);
//
//		/**
//		* Reload the material using the given file path
//		* @param p_material
//		* @param p_path
//		*/
//		static void Reload(LitchiRuntime::Material& p_material, const std::string& p_path);
//
//		/**
//		* Save the material to the given path
//		* @param p_material
//		* @param p_path
//		*/
//		static void Save(LitchiRuntime::Material& p_material, const std::string& p_path);
//
//		/**
//		* Destroy the given material
//		* @param p_material
//		*/
//		static bool Destroy(LitchiRuntime::Material*& p_material);
//	};
//}
