#ifndef UNTITLED_SCENEMANAGER_H
#define UNTITLED_SCENEMANAGER_H

#include <map>
#include <string>
#include "Runtime/Core/DataStruct/tree.h"

namespace LitchiRuntime
{
	class GameObject;
	class Scene
	{
	public:
		Scene();
		Scene(std::string name);
		~Scene();

		std::string GetName(){ return name_; }
		void SetName(std::string name){name_ = name; }

		void AddGameObject(GameObject* game_object);

		/// ����GameObject���ṹ
		/// \return
		Tree& game_object_tree() { return game_object_tree_; }

		/// ����GameObject
		/// \param func
		void Foreach(std::function<void(GameObject* game_object)> func);

		/// ȫ�ֲ���GameObject
		/// \param name
		/// \return
		GameObject* Find(const char* name);

		std::vector<GameObject*> game_object_vec_; //�洢���е�GameObject��

	private:
		std::string name_; //��������

		Tree game_object_tree_; //�����洢���е�GameObject��

	};


	class SceneManager
	{
	public:

		SceneManager(std::string sceneRootFolderPath);
		~SceneManager();

		bool LoadScene(std::string path);

		Scene* CreateScene(std::string sceneName);

		bool DestroyScene(std::string sceneName);

		Scene* GetScene(std::string sceneName)
		{
			return scene_map_[sceneName];
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
			if(path.empty())
			{
				m_currentSceneLoadedFromPath = false;
				m_currentSceneSourcePath = "";
				
			}else
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

		/// ��������Scene��GameObject
		/// \param func
		void Foreach(std::function<void(GameObject* game_object)> func);
		void SaveCurrentSceneTo(const std::string& path);

	private:

		std::string m_sceneRootFolderPath;

		 std::map<std::string, Scene*> scene_map_;
		Scene* m_currScene = nullptr;
		bool m_currentSceneLoadedFromPath = false;
		std::string m_currentSceneSourcePath = "";

	};
}
#endif //UNTITLED_TYPE_H
