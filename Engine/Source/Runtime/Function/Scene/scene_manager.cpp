#include "scene_manager.h"

#include "Runtime/Core/Meta/Serializer/serializer.h"
#include "Runtime/Function/Framework/GameObject/game_object.h"
#include "Runtime/Resource/asset_manager.h"

namespace LitchiRuntime
{
	Scene::Scene() :name_("default")
	{
	}
	Scene::Scene(std::string name)
	{
		name_ = name;
	}
	Scene::~Scene()
	{
		// 析构所有的GO
		for (auto iter = game_object_vec_.begin(); iter != game_object_vec_.end(); iter++) {
			GameObject* game_object = *iter;
			delete game_object;
		}
		game_object_vec_.clear();
	}

	GameObject* Scene::CreateGameObject(std::string name)
	{
		auto* game_object = new GameObject(name, availableID++);
		game_object->SetScene(this);

		// 将go添加到game_object_vec_中
		game_object_vec_.push_back(game_object);
		game_object_tree_.root_node()->AddChild(game_object);

		return game_object;
	}

	void Scene::Foreach(std::function<void(GameObject* game_object)> func)
	{
		game_object_tree_.Post(game_object_tree_.root_node(), [&func](Tree::Node* node) {
			auto n = node;
			GameObject* game_object = dynamic_cast<GameObject*>(n);
			func(game_object);
			});
	}
	GameObject* Scene::Find(const char* name) {
		GameObject* game_object_find = nullptr;
		game_object_tree_.Find(game_object_tree_.root_node(), [&name](Tree::Node* node) {
			GameObject* game_object = dynamic_cast<GameObject*>(node);
			if (game_object->GetName() == name) {
				return true;
			}
			return false;
			}, reinterpret_cast<Tree::Node**>(&game_object_find));
		return game_object_find;
	}

	GameObject* Scene::Find(const int64_t id)
	{
		GameObject* game_object_find = nullptr;
		game_object_tree_.Find(game_object_tree_.root_node(), [&id](Tree::Node* node) {
			GameObject* game_object = dynamic_cast<GameObject*>(node);
			if (game_object != nullptr && game_object->id == id) {
				return true;
			}
			return false;
			}, reinterpret_cast<Tree::Node**>(&game_object_find));
		return game_object_find;
	}

	void Scene::PostResourceLoaded()
	{
		for (auto go : game_object_vec_)
		{
			game_object_tree_.root_node()->AddChild(go);

			go->PostResourceLoaded();
		}

		for (auto go : game_object_vec_)
		{
			auto* parentGO = Find(go->parentId);
			go->SetParent(parentGO);
		}
	}

	SceneManager::SceneManager(std::string sceneRootFolderPath) :m_sceneRootFolderPath(sceneRootFolderPath)
	{

	}

	SceneManager::~SceneManager()
	{
	}

	bool SceneManager::LoadScene(std::string path)
	{
		std::string completePath = m_sceneRootFolderPath + path;
		auto* scene = new Scene("Temp");
		if (!AssetManager::LoadAsset(completePath, *scene))
		{
			return false;
		}

		scene->PostResourceLoaded();

		// 将scene添加到map中
		scene_map_[scene->GetName()] = scene;

		// 初始化Scene中所有的GameObject,配置GameObject的层级关系
		SetCurrentSceneSourcePath(completePath);
		SetCurrentScene(scene);

		return true;
	}

	void SceneManager::SaveCurrentScene(const std::string& completePath)
	{
		AssetManager::SaveAsset<Scene>(*m_currScene, completePath);

		// 设置当前场景的本地路径
		SetCurrentSceneSourcePath(completePath);
	}

	Scene* SceneManager::CreateScene(std::string sceneName)
	{
		Scene* scene = new Scene(sceneName);

		// 将scene添加到map中
		scene_map_[sceneName] = scene;

		// 初始化Scene中所有的GameObject,配置GameObject的层级关系
		SetCurrentSceneSourcePath("");
		SetCurrentScene(scene);

		return scene;
	}

	bool SceneManager::DestroyScene(std::string sceneName)
	{
		auto scene = scene_map_[sceneName];
		scene_map_.erase(sceneName);
		delete scene;

		return true;
	}

	void SceneManager::Foreach(std::function<void(GameObject* game_object)> func)
	{
		for (auto iter = scene_map_.begin(); iter != scene_map_.end(); iter++) {
			Scene* scene = iter->second;
			scene->Foreach(func);
			//current_camera_->CheckCancelRenderToTexture();
		}
	}


}
