
#include "Runtime/Core/pch.h"
#include "SceneManager.h"

#include <stack>

#include "Runtime/Core/Meta/Serializer/serializer.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include "Runtime/Function/Framework/Component/UI/RectTransform.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Renderer/Rendering/Renderer.h"
#include "Runtime/Function/Scripting/ScriptEngine.h"
#include "Runtime/Resource/AssetManager.h"
#include <Standalone/include/ApplicationStandalone.h>

namespace LitchiRuntime
{
	Event<GameObject*>	Scene::InstantiatePrefabEvent;
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

	GameObject* Scene::CreateGameObject(const std::string& name, bool isUI)
	{
		int64_t id = m_availableID++;
		auto* game_object = new GameObject(name, id,m_isPlaying,this);

		// add default transform
		if(!isUI)
		{
			game_object->AddComponent<Transform>();
		}else
		{
			game_object->AddComponent<RectTransform>();
		}

		m_gameObjectList.push_back(game_object);

		// invoke event
		GameObject::CreatedEvent.Invoke(game_object);

		if(m_isPlaying)
		{
			game_object->SetSleeping(false);
			if(game_object->GetActive())
			{
				game_object->OnAwake();
				game_object->OnEnable();
				game_object->OnStart();
			}
		}

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
			auto go = transform->GetGameObject();


			delete transform->GetGameObject();
		}

		m_resolve = true;
	}

	GameObject* Scene::InstantiatePrefab(Prefab* prefab, GameObject* root)
	{
		// todo: 
		auto prefab_data = AssetManager::Serialize(prefab);

		auto deep_copy_prefab = new Prefab();
		AssetManager::Deserialize(prefab_data, deep_copy_prefab);
		deep_copy_prefab->PostResourceLoaded();

		// move deep copy prefab gameObjects to scene
		// dfs in root entity
		auto rootObject = deep_copy_prefab->GetRootEntity();
		if(root!=nullptr)
		{
			rootObject->SetParent(root);
		}
		std::stack<GameObject*> stack;
		stack.push(rootObject);
		while (stack.size() > 0)
		{
			auto root = stack.top();
			if(root==nullptr)
			{
				break;
			}
			stack.pop();

			auto newId = m_availableID++;
			root->m_id = newId;
			root->SetScene(this);
			m_gameObjectList.push_back(root);

			if (m_isPlaying)
			{
				root->SetSleeping(false);
				if (root->GetActive())
				{
					root->OnAwake();
					root->OnEnable();
					root->OnStart();
				}
			}
			/*root->ForeachComponent([](Component* comp) {
				comp->OnAwake();
				});*/

			if(root->GetChildren().empty())
			{
				continue;
			}

			auto childs = root->GetChildren();
			for (auto data : childs)
			{
				data->m_parentId = newId;
				stack.push(data);
			}
		}
		deep_copy_prefab->OnlyClearOnDeepCopy();
		delete deep_copy_prefab;

		m_resolve = true;

		InstantiatePrefabEvent.Invoke(rootObject);
		
		// return root entity
		return rootObject;
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

	void Scene::OnEditorUpdate()
	{
		/* Wake up actors to allow them to react to OnEnable, OnDisable and OnDestroy, */
		std::for_each(m_gameObjectList.begin(), m_gameObjectList.end(), [](GameObject* p_element)
			{ p_element->OnEditorUpdate(); });
	}

	void Scene::Play()
	{
		m_isPlaying = true;

		/* Wake up actors to allow them to react to OnEnable, OnDisable and OnDestroy, */
		std::for_each(m_gameObjectList.begin(), m_gameObjectList.end(), [](GameObject* p_element)
			{ p_element->SetSleeping(false); });

		std::for_each(m_gameObjectList.begin(), m_gameObjectList.end(), [](GameObject* p_element)
			{ if (p_element->GetActive()) p_element->ForeachComponent([](Component* comp) {comp->OnAwake(); }); });
		std::for_each(m_gameObjectList.begin(), m_gameObjectList.end(), [](GameObject* p_element) 
			{ if (p_element->GetActive()) p_element->ForeachComponent([](Component* comp) {comp->OnEnable(); }); });
		std::for_each(m_gameObjectList.begin(), m_gameObjectList.end(), [](GameObject* p_element)
			{ if (p_element->GetActive()) p_element->OnStart(); });
	}

	void Scene::Update()
	{
		// Start
		std::for_each(m_gameObjectList.begin(), m_gameObjectList.end(), [](GameObject* p_element) {  p_element->OnUpdate(); });
	}

	void Scene::FixedUpdate()
	{
		std::for_each(m_gameObjectList.begin(), m_gameObjectList.end(), [](GameObject* p_element) {  p_element->OnFixedUpdate(); });
	}

	void Scene::LateUpdate()
	{
		std::for_each(m_gameObjectList.begin(), m_gameObjectList.end(), [](GameObject* p_element) {  p_element->OnLateUpdate(); });
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
			go->SetScene(this);
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
		//for (auto m_scene_map : m_sceneList)
		//{
		//	delete m_scene_map;
		//}

		//m_sceneList.clear();
		UnloadCurrentScene();
	}

	void SceneManager::LoadEmptyScene()
	{
		CreateEmptyScene();
		SceneLoadEvent.Invoke();
	}

	void SceneManager::CreateEmptyScene()
	{
		UnloadCurrentScene();

		m_currScene = new Scene("Empty Scene");

	}

	bool SceneManager::LoadScene(const std::string& path)
	{
		CreateEmptyScene();

		if(!AssetManager::LoadAsset(ApplicationBase::Instance()->configManager->GetAssetFolderFullPath()+path, m_currScene))
		{
			UnloadCurrentScene();
			return false;
		}

		StoreCurrentSceneSourcePath(path);

		m_currScene->PostResourceLoaded();

		SceneLoadEvent.Invoke();

		return true;
	}

	bool SceneManager::LoadSceneFromMemory(std::string& p_doc)
	{
		CreateEmptyScene();

		if(!Serializer::DeserializeFromJson(p_doc,m_currScene))
		{
			UnloadCurrentScene();
			return false;
		}

		m_currScene->PostResourceLoaded();

		SceneLoadEvent.Invoke();

		return true;
	}

	void SceneManager::UnloadCurrentScene()
	{
		if(m_currScene)
		{
			delete m_currScene;
			m_currScene = nullptr;
			SceneUnloadEvent.Invoke();
		}

		ForgetCurrentSceneSourcePath();
	}

	bool SceneManager::HasCurrentScene() const
	{
		return m_currScene!=nullptr;
	}

	//bool SceneManager::LoadScene(std::string path)
	//{
	//	std::string completePath = path;
	//	auto* scene = new Scene("Temp");
	//	if (!AssetManager::LoadAsset(completePath, *scene))
	//	{
	//		return false;
	//	}

	//	scene->PostResourceLoaded();

	//	// 将scene添加到map中
	//	m_sceneList.push_back(scene);

	//	// 初始化Scene中所有的GameObject,配置GameObject的层级关系
	//	SetCurrentSceneSourcePath(completePath);
	//	SetCurrentScene(scene);

	//	// todo: is game play, need Awake all comp

	//	// todo 将Scene注册到ScriptEngine中

	//	return true;
	//}

	void SceneManager::SaveCurrentScene(const std::string& path)
	{
		AssetManager::SaveAsset<Scene>(*m_currScene, ApplicationBase::Instance()->configManager->GetAssetFolderFullPath() + path);

		// 设置当前场景的本地路径
		StoreCurrentSceneSourcePath(path);
	}

	//Scene* SceneManager::CreateScene(std::string sceneName)
	//{
	//	Scene* scene = new Scene(sceneName);
	//	scene->PostResourceLoaded();

	//	// 将scene添加到map中
	//	m_sceneList.push_back(scene);

	//	// 初始化Scene中所有的GameObject,配置GameObject的层级关系
	//	SetCurrentSceneSourcePath("");
	//	SetCurrentScene(scene);

	//	return scene;
	//}

	//bool SceneManager::DestroyScene(Scene* scene)
	//{
	//	for (auto iter = m_sceneList.begin(); iter != m_sceneList.end(); iter++) {
	//		if(*iter == scene)
	//		{
	//			m_sceneList.erase(iter);
	//			delete scene;
	//			return true;
	//		}
	//	}

	//	return false;
	//}

	void SceneManager::StoreCurrentSceneSourcePath(const std::string& path)
	{
		m_currentSceneSourcePath = path;
		m_currentSceneLoadedFromPath = true;
		CurrentSceneSourcePathChangedEvent.Invoke(m_currentSceneSourcePath);
	}

	void SceneManager::ForgetCurrentSceneSourcePath()
	{
		m_currentSceneSourcePath = "";
		m_currentSceneLoadedFromPath = false;
		CurrentSceneSourcePathChangedEvent.Invoke(m_currentSceneSourcePath);
	}

	void SceneManager::Foreach(std::function<void(GameObject* game_object)> func)
	{
		//for (auto iter = m_sceneList.begin(); iter != m_sceneList.end(); iter++) {
		//	Scene* scene = *iter;
		//	scene->Foreach(func);
		//	//current_camera_->CheckCancelRenderToTexture();
		//}

		m_currScene->Foreach(func);
	}


}
