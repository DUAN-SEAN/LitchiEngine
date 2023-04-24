
#ifndef UNTITLED_SCENEMANAGER_H
#define UNTITLED_SCENEMANAGER_H

#include "Runtime/Function/Framework/GameObject/game_object.h"

#include "Runtime/Core/DataStruct/tree.h"
namespace LitchiRuntime
{
	class Scene
	{
	public:

		/// 返回GameObject树结构
		/// \return
		Tree& game_object_tree() { return game_object_tree_; }
		/// 遍历GameObject
		/// \param func
		void Foreach(std::function<void(GameObject* game_object)> func);

	private:
		
		std::vector<GameObject> game_object_vec_;// 序列化GameObject

	private:

		Tree game_object_tree_;//用树存储所有的GameObject。

		std::list<GameObject*> game_object_list_;//存储所有的GameObject。
	};


	class SceneManager {
	public:

		bool LoadScene(Scene* scene);

		Scene* CreateScene(std::string sceneName);

		Scene* GetScene(std::string sceneName)
		{
			return scene_map_[sceneName];
		}

	private:
		std::map<std::string,Scene*> scene_map_;
	};
}
#endif //UNTITLED_TYPE_H