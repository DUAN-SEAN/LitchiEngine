#include "SceneManager.h"

#include "Runtime/Core/Meta/Serializer/serializer.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Renderer/Rendering/Renderer.h"
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

		m_resolve = true;

		return game_object;
	}

	void Scene::RemoveGameObject(GameObject* go)
	{
		// todo: 考虑层级关系, 递归移除

		std::vector<Transform*> entities_to_remove;
		auto tran = go->GetComponent<Transform>();
		entities_to_remove.push_back(tran);  // Add the root entity
		tran->GetDescendants(&entities_to_remove); // Get descendants

		// Create a set containing the object IDs of entities to remove
		std::set<uint64_t> ids_to_remove;
		for (Transform* transform : entities_to_remove)
		{
			ids_to_remove.insert(transform->GetGameObject()->GetObjectId());
		}

		// Remove entities using a single loop
		m_gameObjectList.erase(std::remove_if(m_gameObjectList.begin(), m_gameObjectList.end(),
		                                      [&](GameObject* entity)
		                                      {
			                                      return ids_to_remove.count(entity->GetObjectId()) > 0;
		                                      }),
			m_gameObjectList.end());

		// If there was a parent, update it
		if (Transform* parent = tran->GetParent())
		{
			parent->AcquireChildren();
		}

		// delete gameObject
		for (Transform* transform : entities_to_remove)
		{
			delete transform->GetGameObject();
		}

		m_resolve = true;
	}

	void Scene::Foreach(std::function<void(GameObject* game_object)> func)
	{
		for (auto go : m_gameObjectList)
		{
			func(go);
		}
	}

	GameObject* Scene::Find(const char* name) {
		for (auto go : m_gameObjectList)
		{
			if (go->GetName() == name) {
				return go;
			}
		}

		return nullptr;
	}

	GameObject* Scene::Find(const int64_t id)
	{
		for (auto go : m_gameObjectList)
		{
			if (go->m_id == id) {
				return go;
			}
		}

		return nullptr;
	}

	GameObject* Scene::FindByUnmanagedId(const int64_t unmanagedId)
	{
		for (auto go : m_gameObjectList)
		{
			if (go->GetUnmanagedId() == unmanagedId) {
				return go;
			}
		}

		return nullptr;
	}

	std::vector<GameObject*> Scene::GetRootGameObjectList()
	{
		std::vector<GameObject*> root_entities;
		for (auto go : m_gameObjectList)
		{
			if (go->GetComponent<Transform>()->IsRoot())
			{
				root_entities.emplace_back(go);
			}
		}

		return root_entities;
	}

	void Scene::Update()
	{
		// Start


		// Update
		for (auto* entity : m_gameObjectList)
		{
			for (auto* comp :entity->GetComponents())
			{
				comp->Update();
			}
		}
	}

	void Scene::FixedUpdate()
	{
		for (auto* entity : m_gameObjectList)
		{
			for (auto* comp : entity->GetComponents())
			{
				comp->FixedUpdate();
			}
		}
	}

	void Scene::ResetResolve()
	{
		if (m_resolve)
		{
			m_resolve = false;
		}
	}

	void Scene::PostResourceLoaded()
	{
		//// 创建非托管对象
		//m_unmanagedId = ScriptEngine::CreateScene();

		for (auto go : m_gameObjectList)
		{
			go->PostResourceLoaded();
		}

		for (auto go : m_gameObjectList)
		{
			auto* parentGO = Find(go->m_parentId);
			go->SetParent(parentGO);
		}
	}

	SceneManager::SceneManager()
	{

	}

	SceneManager::~SceneManager()
	{
		for (auto m_scene_map : m_sceneList)
		{
			delete m_scene_map;
		}

		m_sceneList.clear();
	}

	bool SceneManager::LoadScene(std::string path)
	{
		std::string completePath = path;
		auto* scene = new Scene("Temp");
		if (!AssetManager::LoadAsset(completePath, *scene))
		{
			return false;
		}

		scene->PostResourceLoaded();

		// 将scene添加到map中
		m_sceneList.push_back(scene);

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
		m_sceneList.push_back(scene);

		// 初始化Scene中所有的GameObject,配置GameObject的层级关系
		SetCurrentSceneSourcePath("");
		SetCurrentScene(scene);

		return scene;
	}

	bool SceneManager::DestroyScene(Scene* scene)
	{
		for (auto iter = m_sceneList.begin(); iter != m_sceneList.end(); iter++) {
			if(*iter == scene)
			{
				m_sceneList.erase(iter);
				delete scene;
				return true;
			}
		}

		return false;
	}

	void SceneManager::Foreach(std::function<void(GameObject* game_object)> func)
	{
		for (auto iter = m_sceneList.begin(); iter != m_sceneList.end(); iter++) {
			Scene* scene = *iter;
			scene->Foreach(func);
			//current_camera_->CheckCancelRenderToTexture();
		}
	}


}
