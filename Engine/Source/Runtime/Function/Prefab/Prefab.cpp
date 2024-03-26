
#include "Runtime/Core/pch.h"
#include "Prefab.h"

#include "Runtime/Core/App/ApplicationBase.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include "Runtime/Function/Framework/Component/UI/RectTransform.h"
#include "Runtime/Resource/AssetManager.h"

namespace LitchiRuntime
{
	Prefab::Prefab() :m_name("default")
	{
	}
	Prefab::Prefab(std::string name)
	{
		m_name = name;
	}
	Prefab::~Prefab()
	{
		// 析构所有的GO
		for (auto iter = m_gameObjectList.begin(); iter != m_gameObjectList.end(); iter++) {
			GameObject* game_object = *iter;
			delete game_object;
		}
		m_gameObjectList.clear();
	}

	bool Prefab::LoadFromFile(const std::string& path)
	{
		Prefab* prefab = this;
		if(!AssetManager::LoadAsset(path, prefab))
		{
			return false;
		}

		return true;
	}

	bool Prefab::SaveToFile(const std::string& path)
	{
		if (!AssetManager::SaveAsset(this, path))
		{
			return false;
		}

		return true;
	}

	void Prefab::OnlyClearOnDeepCopy()
	{
		m_gameObjectList.clear();
	}

	GameObject* Prefab::CreateGameObject(const std::string& name, bool isUI)
	{
		auto id = m_availableID++;
		bool isPlaying = false;
		auto* game_object = new GameObject(name, id, isPlaying,nullptr);
		if (!isUI)
		{
			game_object->AddComponent<Transform>();
		}
		else
		{
			game_object->AddComponent<RectTransform>();
		}

		// 将go添加到game_object_vec_中
		m_gameObjectList.push_back(game_object);

		// invoke event
		GameObject::CreatedEvent.Invoke(game_object);

		return game_object;
	}

	void Prefab::RemoveGameObject(GameObject* go)
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
	}

	GameObject* Prefab::Find(const char* name) {
		for (auto go : m_gameObjectList)
		{
			if (go->GetName() == name) {
				return go;
			}
		}

		return nullptr;
	}

	GameObject* Prefab::Find(const int64_t id)
	{
		for (auto go : m_gameObjectList)
		{
			if (go->m_id == id) {
				return go;
			}
		}

		return nullptr;
	}

	GameObject* Prefab::FindByUnmanagedId(const int64_t unmanagedId)
	{
		for (auto go : m_gameObjectList)
		{
			if (go->GetUnmanagedId() == unmanagedId) {
				return go;
			}
		}

		return nullptr;
	}

	std::vector<GameObject*> Prefab::GetRootGameObjectList()
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

	void Prefab::PostResourceLoaded()
	{
		// todo:
		// find root entity
		auto rootEntity = Find(m_root_entity_id);
		if(!rootEntity)
		{
			DEBUG_LOG_ERROR("Cant Find Root Entity");
			return;
		}

		m_root_entity = rootEntity;

		for (auto go : m_gameObjectList)
		{
			go->SetScene(nullptr);
			go->PostResourceLoaded();
		}
		//
		for (auto go : m_gameObjectList)
		{
			auto* parentGO = Find(go->m_parentId);
			go->SetParent(parentGO);
		}
	}
}
