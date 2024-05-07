
#pragma once

#include <unordered_map>
#include <any>
#include <string>

namespace LitchiRuntime
{
	/**
	* Handle the management of various resources of variable type
	*/
	template<typename T>
	class AResourceManager
	{
	public:
		/**
		* Handle the creation of a resource and register it
		* @param p_path
		*/
		T* LoadResource(const std::string& p_path);

		/**
		* Handle the destruction of a resource and unregister it
		* @param p_path
		*/
		void UnloadResource(const std::string& p_path);

		/**
		* Move a resource to the new given id (path)
		* @param p_previousPath
		* @param p_newPath
		*/
		bool MoveResource(const std::string& p_previousPath, const std::string& p_newPath);

		/**
		* Reload a resource
		* @param p_path
		*/
		void ReloadResource(const std::string& p_path);

		/**
		* Return true if the resource exists (= Is registered)
		* @param p_path
		*/
		bool IsResourceRegistered(const std::string& p_path);

		/**
		* Destroy and unregister every resources
		*/
		void UnloadResources();

		/**
		* Register a resource and associate it with a given path.
		* After this method is called, the memory managment of the resource
		* belong the the Resource Manager (Pointer memory deallocation)
		* @param p_path
		* @param p_instance
		*/
		T* RegisterResource(const std::string& p_path, T* p_instance);

		/**
		* Unregister a resource (You have to destroy the resource before calling this method
		* to prevent memory leaks)
		* @param p_path
		*/
		void UnregisterResource(const std::string& p_path);

		/**
		* Return the instance linked to the given path or try to load it if not registered.
		* Otherwise it will return nullptr.
		* @param p_path
		* @param p_tryToLoadIfNotFound
		*/
		T* GetResource(const std::string& p_path, bool p_tryToLoadIfNotFound = true);

		/**
		* Operator overload to get an instance linked to the given path.
		* @note See GetResource for more informations
		* @param p_path
		*/
		T* operator[](const std::string& p_path);

		/**
		* Returns the resource map
		*/
		std::unordered_map<std::string, T*>& GetResources();

	protected:
		virtual T* CreateResource(const std::string& p_path) = 0;
		virtual void DestroyResource(T* p_resource) = 0;
		virtual void ReloadResource(T* p_resource, const std::string& p_path) = 0;
		std::string GetRealPath(const std::string& p_path) const;
		std::string GetRelativePath(const std::string& p_path) const;

	private:

		std::unordered_map<std::string, T*> m_resources;
	};
}

#include "AResourceManager.inl"