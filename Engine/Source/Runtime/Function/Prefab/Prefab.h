#pragma once

#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Renderer/Resource/IResource.h"

namespace LitchiRuntime
{
	class GameObject;

	// Prefab, A Tree of GameObjects. Use For Mesh and Scene
	class Prefab :public ScriptObject,public IResource
	{
	public:
		Prefab();
		Prefab(const std::string& path);
		~Prefab();
		
		bool LoadFromFile(const std::string& string);
		bool SaveToFile(const std::string& string);

		std::string GetName() { return m_name; }
		void SetName(std::string name) { m_name = name; }

		void OnlyClearOnDeepCopy();

		/**
		 * ����GameObject
		 * \param name go������
		 * \return go
		 */
		GameObject* CreateGameObject(const std::string& name,bool isUI = false);
		void RemoveGameObject(GameObject* go);
		
		/// ȫ�ֲ���GameObject
		/// \param name
		/// \return
		GameObject* Find(const char* name);

		/// ȫ�ֲ���GameObject
		/// \param id
		/// \return
		GameObject* Find(const int64_t id);

		/// ȫ�ֲ���GameObject
		/// \param unmanagedId ���й�ΨһId
		/// \return
		GameObject* FindByUnmanagedId(const int64_t unmanagedId);

		std::vector<GameObject*> GetRootGameObjectList();
		std::vector<GameObject*>& GetAllGameObjectList()
		{
			return m_gameObjectList;
		}

		// get root entity
		void SetRootEntity(GameObject* entity)
		{
			if(GameObject* go = Find(entity->m_id))
			{
				if(go == entity)
				{
					m_root_entity = entity;
					m_root_entity_id = m_root_entity->m_id;
				}
			}
			
		}
		GameObject* GetRootEntity() { return m_root_entity; }

	public:

		std::vector<GameObject*> m_gameObjectList; //�洢���е�GameObject��

		/**
		 * \brief ���õ�id ����id����
		 */
		int64_t m_availableID = 1;

		// root game object id
		int64_t m_root_entity_id;

		void PostResourceLoaded() override;

	private:
		GameObject* m_root_entity;
		std::string m_name;
	};


}
