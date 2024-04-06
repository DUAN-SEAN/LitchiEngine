
#pragma once

#include <map>
#include <string>

#include "Runtime/Core/Tools/Eventing/Event.h"
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

		// Play Game
		void Play();

		/**
		* Returns true if the scene is playing
		* only PlayMode or Runtime
		*/
		bool IsPlaying() const { return m_isPlaying; }

		/**
		* Update every actors
		* @param p_deltaTime
		*/
		void Update();

		/**
		* Update every actors 60 frames per seconds
		* @param p_deltaTime
		*/
		void FixedUpdate();
		/**
		* Update every actors lately
		* @param p_deltaTime
		*/
		void LateUpdate();

		/**
		 * 创建GameObject
		 * \param name go的名字
		 * \return go
		 */
		GameObject* CreateGameObject(const std::string& name,bool isUI = false);
		void RemoveGameObject(GameObject* go);

		// Prefab
		GameObject* InstantiatePrefab(Prefab* prefab,GameObject* root);

		/// 遍历GameObject
		/// \param func
		void Foreach(std::function<void(GameObject* game_object)> func);

		std::string GetName() { return m_name; }
		void SetName(std::string name) { m_name = name; }

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

		void OnEditorUpdate();
	public:

		static Event<GameObject*>	InstantiatePrefabEvent;

	public:

		std::vector<GameObject*> m_gameObjectList{}; //存储所有的GameObject。

		/**
		 * \brief 可用的id 用于id分配
		 */
		int64_t m_availableID = 1;

		void PostResourceLoaded() override;

	private:
		bool m_isPlaying = false;
		std::string m_name; //场景名字
		bool m_resolve = false;
		
	};


	class SceneManager
	{
	public:

		SceneManager();
		~SceneManager();

		// new api
		//  Load an empty scene in memory 
		void LoadEmptyScene();

		// new api
		//  Load an empty scene in memory 
		void CreateEmptyScene();

		// Load specific scene in memory
		bool LoadScene(const std::string& path);

		// Load specific scene in memory
		bool LoadSceneFromMemory(std::string& p_doc);

		// Destroy current scene from memory
		void UnloadCurrentScene();

		// Return true if a scene is currently loaded
		bool HasCurrentScene() const;

		//// will delete api
		//bool LoadScene(std::string path);

		//Scene* CreateScene(std::string sceneName);

		//bool DestroyScene(Scene* scene);
		//
		//void SetCurrentScene(Scene* scene)
		//{
		//	m_currScene = scene;
		//}

		void SaveCurrentScene(const std::string& path);

		Scene* GetCurrentScene()
		{
			return m_currScene;
		}


		std::string GetCurrentSceneSourcePath() const
		{
			return m_currentSceneSourcePath;
		}

		bool IsCurrentSceneLoadedFromPath()
		{
			return m_currentSceneLoadedFromPath;
		}

		void StoreCurrentSceneSourcePath(const std::string& path);

		// Reset the current scene source path to an empty string 
		void ForgetCurrentSceneSourcePath();

		// 遍历所有Scene的GameObject
		void Foreach(std::function<void(GameObject* game_object)> func);

	public:
		Event<> SceneLoadEvent;
		Event<> SceneUnloadEvent;
		Event<const std::string&> CurrentSceneSourcePathChangedEvent;
	private:

		// std::vector<Scene*> m_sceneList;

		Scene* m_currScene {nullptr};
		bool m_currentSceneLoadedFromPath {false};
		std::string m_currentSceneSourcePath {};

	};
}
