#include "SceneManager.h"

#include "Runtime/Core/Meta/Serializer/serializer.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Scripting/ScriptEngine.h"
#include "Runtime/Resource/AssetManager.h"

namespace LitchiRuntime
{
	Scene::Scene() :m_name("default")
	{
	}
	Scene::Scene(std::string name)
	{
		m_name = name;
	}
	Scene::~Scene()
	{
		// 析构所有的GO
		for (auto iter = m_gameObjectList.begin(); iter != m_gameObjectList.end(); iter++) {
			GameObject* game_object = *iter;
			delete game_object;
		}
		m_gameObjectList.clear();
	}

	GameObject* Scene::CreateGameObject(std::string name)
	{
		auto* game_object = new GameObject(name, m_availableID++);
		game_object->SetScene(this);

		// 将go添加到game_object_vec_中
		m_gameObjectList.push_back(game_object);
		m_gameObjectTree.GetRootNode()->AddChildNode(game_object);

		return game_object;
	}

	void Scene::Foreach(std::function<void(GameObject* game_object)> func)
	{
		m_gameObjectTree.Post(m_gameObjectTree.GetRootNode(), [&func](Tree::Node* node) {
			auto n = node;
			GameObject* game_object = dynamic_cast<GameObject*>(n);
			func(game_object);
			});
	}
	GameObject* Scene::Find(const char* name) {
		GameObject* game_object_find = nullptr;
		m_gameObjectTree.Find(m_gameObjectTree.GetRootNode(), [&name](Tree::Node* node) {
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
		m_gameObjectTree.Find(m_gameObjectTree.GetRootNode(), [&id](Tree::Node* node) {
			GameObject* game_object = dynamic_cast<GameObject*>(node);
			if (game_object != nullptr && game_object->m_id == id) {
				return true;
			}
			return false;
			}, reinterpret_cast<Tree::Node**>(&game_object_find));
		return game_object_find;
	}

	GameObject* Scene::FindByUnmanagedId(const int64_t unmanagedId)
	{
		GameObject* game_object_find = nullptr;
		m_gameObjectTree.Find(m_gameObjectTree.GetRootNode(), [&unmanagedId](Tree::Node* node) {
			GameObject* game_object = dynamic_cast<GameObject*>(node);
			if (game_object != nullptr && game_object->GetUnmanagedId() == unmanagedId) {
				return true;
			}
			return false;
			}, reinterpret_cast<Tree::Node**>(&game_object_find));

		return game_object_find;
	}

	void Scene::PostResourceLoaded()
	{
		// 创建非托管对象
		m_unmanagedId = ScriptEngine::CreateScene();

		for (auto go : m_gameObjectList)
		{
			m_gameObjectTree.GetRootNode()->AddChildNode(go);

			go->PostResourceLoaded();
		}

		for (auto go : m_gameObjectList)
		{
			auto* parentGO = Find(go->m_parentId);
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
		m_sceneMap[scene->GetName()] = scene;

		// 初始化Scene中所有的GameObject,配置GameObject的层级关系
		SetCurrentSceneSourcePath(completePath);
		SetCurrentScene(scene);


		// todo 将Scene注册到ScriptEngine中

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
		scene->PostResourceLoaded();

		// 将scene添加到map中
		m_sceneMap[sceneName] = scene;

		// 初始化Scene中所有的GameObject,配置GameObject的层级关系
		SetCurrentSceneSourcePath("");
		SetCurrentScene(scene);

		return scene;
	}

	bool SceneManager::DestroyScene(std::string sceneName)
	{
		auto scene = m_sceneMap[sceneName];
		m_sceneMap.erase(sceneName);
		delete scene;

		return true;
	}

	void SceneManager::Foreach(std::function<void(GameObject* game_object)> func)
	{
		for (auto iter = m_sceneMap.begin(); iter != m_sceneMap.end(); iter++) {
			Scene* scene = iter->second;
			scene->Foreach(func);
			//current_camera_->CheckCancelRenderToTexture();
		}
	}


}
