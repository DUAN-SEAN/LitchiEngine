#pragma once

#include "Runtime/Function/Framework/GameObject/GameObject.h"

namespace LitchiRuntime
{
	class GameObject;
	class Prefab :public ScriptObject
	{
	public:
		Prefab();
		Prefab(std::string name);
		~Prefab();
		
		bool LoadFromFile(const std::string& string);
		bool SaveToFile(const std::string& string);

		std::string GetName() { return m_name; }
		void SetName(std::string name) { m_name = name; }

		/**
		 * ����GameObject
		 * \param name go������
		 * \return go
		 */
		GameObject* CreateGameObject(std::string name);
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

	public:

		std::vector<GameObject*> m_gameObjectList; //�洢���е�GameObject��

		/**
		 * \brief ���õ�id ����id����
		 */
		int64_t m_availableID = 1;

		void PostResourceLoaded() override;

	private:
		std::string m_name;

	};


}
