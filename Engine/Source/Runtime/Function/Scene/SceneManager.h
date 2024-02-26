﻿
#pragma once

#include <map>
#include <string>

#include "Runtime/Function/Prefab/Prefab.h"
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
		GameObject* CreateGameObject(std::string name,bool isUI = false);
		void RemoveGameObject(GameObject* go);

		// Prefab
		GameObject* InstantiatePrefab(Prefab* prefab,GameObject* root);

		/// 遍历GameObject
		/// \param func
		void Foreach(std::function<void(GameObject* game_object)> func);

		void Update();
		void FixedUpdate();

		void Resolve() { m_resolve = true; }
		bool IsNeedResolve() { return m_resolve; }
		void ResetResolve();

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

		void Play();


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

		// new api
		/**
		* Load an empty scene in memory
		*/
		void LoadEmptyScene();


		/**
		* Load specific scene in memory
		* @param p_scenePath
		* @param p_absolute (If this setting is set to true, the scene loader will ignore the "SceneRootFolder" given on SceneManager construction)
		*/
		bool LoadScene(const std::string& p_path, bool p_absolute = false);

		/**
		* Load specific scene in memory
		* @param p_scenePath
		*/
		bool LoadSceneFromMemory(std::string& p_doc);

		/**
		* Destroy current scene from memory
		*/
		void UnloadCurrentScene();


		/**
		* Return true if a scene is currently loaded
		*/
		bool HasCurrentScene() const;

		// will delete api
		bool LoadScene(std::string path);

		Scene* CreateScene(std::string sceneName);

		bool DestroyScene(Scene* scene);
		
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
		
		std::vector<Scene*> m_sceneList;
		Scene* m_currScene = nullptr;
		bool m_currentSceneLoadedFromPath = false;
		std::string m_currentSceneSourcePath = "";

	};
}
