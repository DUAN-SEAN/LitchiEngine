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

		/// 返回GameObject树结构
		/// \return
		Tree& game_object_tree() { return game_object_tree_; }

		/// 遍历GameObject
		/// \param func
		void Foreach(std::function<void(GameObject* game_object)> func);

		/// 全局查找GameObject
		/// \param name
		/// \return
		GameObject* Find(const char* name);

		std::vector<GameObject*> game_object_vec_; //存储所有的GameObject。

	private:
		std::string name_; //场景名字

		Tree game_object_tree_; //用树存储所有的GameObject。

	};


	class SceneManager
	{
	public:

		static bool LoadScene(Scene* scene);

		static Scene* CreateScene(std::string sceneName);

		static bool DestroyScene(std::string sceneName);

		static Scene* GetScene(std::string sceneName)
		{
			return scene_map_[sceneName];
		}

		static void SetCurrentScene(Scene* scene)
		{
			s_scene = scene;
		}

		static Scene* GetCurrentScene()
		{
			return s_scene;
		}

		/// 遍历所有Scene的GameObject
		/// \param func
		static void Foreach(std::function<void(GameObject* game_object)> func);

	private:
		static std::map<std::string, Scene*> scene_map_;

		inline static Scene* s_scene = nullptr;
	};
}
#endif //UNTITLED_TYPE_H
