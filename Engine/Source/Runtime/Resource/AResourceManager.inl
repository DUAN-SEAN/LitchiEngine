
#pragma once

#include <algorithm>

#include "AResourceManager.h"
#include "Runtime/Core/Tools/FileSystem/FileSystem.h"
#include "Runtime/Core/Log/debug.h"

namespace LitchiRuntime
{
	template<typename T>
	inline T* AResourceManager<T>::LoadResource(const std::string & p_path)
	{
		std::string resPath = GetRelativePath(p_path);

		if (auto resource = GetResource(resPath, false); resource)
			return resource;
		else
		{
			auto newResource = CreateResource(resPath);
			if (newResource)
				return RegisterResource(resPath, newResource);
			else
				return nullptr;
		}
	}

	template<typename T>
	inline void AResourceManager<T>::UnloadResource(const std::string & p_path)
	{
		std::string resPath = GetRelativePath(p_path);

		if (auto resource = GetResource(resPath, false); resource)
		{
			auto tempPath = resPath;
			DestroyResource(resource);
			UnregisterResource(tempPath);
		}
	}

	template<typename T>
	inline bool AResourceManager<T>::MoveResource(const std::string & p_previousPath, const std::string & p_newPath)
	{
		if (IsResourceRegistered(p_previousPath) && !IsResourceRegistered(p_newPath))
		{
			T* toMove = m_resources.at(p_previousPath);
			UnregisterResource(p_previousPath);
			RegisterResource(p_newPath, toMove);
			return true;
		}

		return false;
	}

	template<typename T>
	inline void AResourceManager<T>::ReloadResource(const std::string& p_path)
	{
		std::string resPath = GetRelativePath(p_path);

		if (auto resource = GetResource(resPath, false); resource)
		{
			ReloadResource(resource, resPath);
		}
	}

	template<typename T>
	inline bool AResourceManager<T>::IsResourceRegistered(const std::string & p_path)
	{
		std::string resPath = GetRelativePath(p_path);
		return m_resources.find(resPath) != m_resources.end();
	}

	template<typename T>
	inline void AResourceManager<T>::UnloadResources()
	{
		for (auto&[key, value] : m_resources)
			DestroyResource(value);

		m_resources.clear();
	}

	template<typename T>
	inline T* AResourceManager<T>::RegisterResource(const std::string& p_path, T* p_instance)
	{
		if (auto resource = GetResource(p_path, false); resource)
			DestroyResource(resource);

		DEBUG_LOG_INFO("RegisterResource path:{}", p_path);

		m_resources[p_path] = p_instance;

		return p_instance;
	}

	template<typename T>
	inline void AResourceManager<T>::UnregisterResource(const std::string & p_path)
	{
		DEBUG_LOG_INFO("UnregisterResource path:{}", p_path);

		m_resources.erase(p_path);
	}

	template<typename T>
	inline T* AResourceManager<T>::GetResource(const std::string& p_path, bool p_tryToLoadIfNotFound)
	{
		if (auto resource = m_resources.find(p_path); resource != m_resources.end())
		{
			return resource->second;
		}
		else if (p_tryToLoadIfNotFound)
		{
			return LoadResource(p_path);
		}

		return nullptr;
	}

	template<typename T>
	inline T* AResourceManager<T>::operator[](const std::string & p_path)
	{
		return GetResource(p_path);
	}

	template<typename T>
	inline std::unordered_map<std::string, T*>& AResourceManager<T>::GetResources()
	{
		return m_resources;
	}

	template<typename T>
	inline std::string AResourceManager<T>::GetRealPath(const std::string& p_path) const
	{
		std::string result;

		if(FileSystem::IsFullPath(p_path))
		{
			return p_path;
		}

		if (p_path[0] == ':') // The path is an engine path
		{
			result = FileSystem::GetEngineAssetDirectoryPath() + std::string(p_path.data() + 1, p_path.data() + p_path.size());
		}
		else // The path is a project path
		{
			result = FileSystem::GetProjectAssetDirectoryPath() + p_path;
		}

		return result;
	}
	template<typename T>
	inline std::string AResourceManager<T>::GetRelativePath(const std::string& p_path) const
	{
		std::string resPath = p_path;
		if (FileSystem::IsFullPath(resPath))
		{
			resPath = FileSystem::GetRelativePathAssetFromNative(p_path);
		}

		return resPath;
	}
}