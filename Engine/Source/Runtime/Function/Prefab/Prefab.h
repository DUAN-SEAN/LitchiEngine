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
		 * 创建GameObject
		 * \param name go的名字
		 * \return go
		 */
		GameObject* CreateGameObject(std::string name);
		void RemoveGameObject(GameObject* go);
		
		/// 全局查找GameObject
		/// \param name
		/// \return
		GameObject* Find(const char* name);

		/// 全局查找GameObject
		/// \param id
		/// \return
		GameObject* Find(const int64_t id);

		/// 全局查找GameObject
		/// \param unmanagedId 非托管唯一Id
		/// \return
		GameObject* FindByUnmanagedId(const int64_t unmanagedId);

		std::vector<GameObject*> GetRootGameObjectList();
		std::vector<GameObject*>& GetAllGameObjectList()
		{
			return m_gameObjectList;
		}

	public:

		std::vector<GameObject*> m_gameObjectList; //存储所有的GameObject。

		/**
		 * \brief 可用的id 用于id分配
		 */
		int64_t m_availableID = 1;

		void PostResourceLoaded() override;

	private:
		std::string m_name;

	};


}
