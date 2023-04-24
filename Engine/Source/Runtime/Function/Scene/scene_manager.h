
#ifndef UNTITLED_SCENEMANAGER_H
#define UNTITLED_SCENEMANAGER_H

#include "Runtime/Function/Framework/GameObject/game_object.h"

#include "Runtime/Core/DataStruct/tree.h"
namespace LitchiRuntime
{
	class Scene
	{
	public:

		/// ����GameObject���ṹ
		/// \return
		Tree& game_object_tree() { return game_object_tree_; }
		/// ����GameObject
		/// \param func
		void Foreach(std::function<void(GameObject* game_object)> func);

	private:
		
		std::vector<GameObject> game_object_vec_;// ���л�GameObject

	private:

		Tree game_object_tree_;//�����洢���е�GameObject��

		std::list<GameObject*> game_object_list_;//�洢���е�GameObject��
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