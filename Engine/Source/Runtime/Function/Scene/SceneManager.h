
#pragma once

#include <map>
#include <string>
#include "Runtime/Core/DataStruct/tree.h"
#include "Runtime/Function/Scripting/ScriptObject.h"

namespace LitchiRuntime
{
	class GameObject;
	class Scene :public ScriptObject
	{
	public:
		Scene();
		Scene(std::string name);
		~Scene();

		std::string GetName() { return m_name; }
		void SetName(std::string name) { m_name = name; }

		/**
		 * 创建GameObject
		 * \param name go的名字
		 * \return go
		 */
		GameObject* CreateGameObject(std::string name);
		void RemoveGameObject(GameObject* go);
		
		/// 遍历GameObject
		/// \param func
		void Foreach(std::function<void(GameObject* game_object)> func);

		void Tick();

		void Resolve() { m_resolve = true; }

		/// 全局查找GameObject
		/// \param name
		/// \return
		GameObject* Find(const char* name);


		/// 全局查找GameObject
		/// \param id
		/// \return
		GameObject* Find(const int64_t id);

		/// 全局查找GameObject
		/// \param unmanagedId 非托管唯一Id
		/// \return
		GameObject* FindByUnmanagedId(const int64_t unmanagedId);

		std::vector<GameObject*> GetRootGameObjectList();
		std::vector<GameObject*>& GetAllGameObjectList()
		{
			return m_gameObjectList;
		}




	public:

		std::vector<GameObject*> m_gameObjectList; //存储所有的GameObject。

		/**
		 * \brief 可用的id 用于id分配
		 */
		int64_t m_availableID = 1;

		void PostResourceLoaded() override;

	private:
		std::string m_name; //场景名字
		bool m_resolve = false;
		
	};


	class SceneManager
	{
	public:

		SceneManager();
		~SceneManager();

		bool LoadScene(std::string path);

		Scene* CreateScene(std::string sceneName);

		bool DestroyScene(std::string sceneName);

		Scene* GetScene(std::string sceneName)
		{
			return m_sceneMap[sceneName];
		}

		void SetCurrentScene(Scene* scene)
		{
			m_currScene = scene;
		}

		Scene* GetCurrentScene()
		{
			return m_currScene;
		}

		void SetCurrentSceneSourcePath(const std::string& path)
		{
			if (path.empty())
			{
				m_currentSceneLoadedFromPath = false;
				m_currentSceneSourcePath = "";

			}
			else
			{

				m_currentSceneLoadedFromPath = true;
				m_currentSceneSourcePath = path;
			}
		}

		std::string GetCurrentSceneSourcePath() const
		{
			return m_currentSceneSourcePath;
		}

		bool IsCurrentSceneLoadedFromPath()
		{
			return m_currentSceneLoadedFromPath;
		}

		/// 遍历所有Scene的GameObject
		/// \param func
		void Foreach(std::function<void(GameObject* game_object)> func);
		void SaveCurrentScene(const std::string& path);

	private:
		
		std::map<std::string, Scene*> m_sceneMap;
		Scene* m_currScene = nullptr;
		bool m_currentSceneLoadedFromPath = false;
		std::string m_currentSceneSourcePath = "";

	};
}
