
#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <list>
#include <functional>
#include "Runtime/Core/DataStruct/tree.h"
#include "Runtime/Function/Framework/Component/Base/component.h"

namespace LitchiRuntime
{
	class Scene;
	class GameObject :public Tree::Node,Object{
	public:
		GameObject() {}
		GameObject(std::string name,int64_t id);
		~GameObject();

		std::string GetName() { return m_name; }
		void SetName(std::string name) { m_name = name; }

		unsigned char GetLayer() { return m_layer; }
		void SetLayer(unsigned char layer) { m_layer = layer; }

		bool GetActive() { return m_active; }
		void SetActive(bool active) { m_active = active; }

		/// 设置父节点
		/// \param parent
		/// \return
		bool SetParent(GameObject* parent);

		/**
		 * \brief 是否有父亲
		 * \return 
		 */
		bool HasParent();

		GameObject* GetParent() { return (GameObject*)GetParentNode(); }

		std::list<GameObject*>& GetChildren()
		{
			std::list<GameObject*> dadList;
			for (auto a : GetChildrenList()) {
				dadList.push_back((GameObject*)a);       //依次加到父类List里
			}

			return dadList;
		}

		virtual void PostResourceLoaded() override;

	public:
		Scene* GetScene();
		void SetScene(Scene* scene);

		/// 添加组件，仅用于C++中添加组件。
		/// \tparam T 组件类型
		/// \return 组件实例
		template <class T = Component>
		T* AddComponent() {
			T* component = new T();
			AttachComponent(component);
			component->Awake();
			return dynamic_cast<T*>(component);
		}

		/// 附加组件实例
		/// \param component_instance_table

		template <class T = Component>
		void AttachComponent(T* component)
		{
			component->SetGameObject(this);
			//获取类名
			type t = type::get<T>();
			std::string component_type_name = t.get_name().to_string();
			
			m_componentList.push_back(component);
			
		}

		/// 获取组件，仅用于C++中。
		/// \tparam T 组件类型
		/// \return 组件实例
		template <class T = Component>
		T* GetComponent() {
			//获取类名
			type t = type::get<T>();
			std::string component_type_name = t.get_name().to_string();
			std::vector<Component*> component_vec;

			for (auto iter = m_componentList.begin(); iter != m_componentList.end(); iter++)
			{
				if ((*iter)->get_type().get_name() == component_type_name)
				{
					return dynamic_cast<T*>(*iter);
				}
			}
			return nullptr;
		}

		 std::vector<Component*>& GetComponents() { return m_componentList; }

		/// 遍历组件
		/// \param func
		void ForeachComponent(std::function<void(Component*)> func);
		bool RemoveComponent(Component* component)
		{
			//获取类名
			type t = component->get_type();
			std::string component_type_name = t.get_name().to_string();
			for (auto iter = m_componentList.begin(); iter != m_componentList.end(); iter++)
			{
				if (*iter == component)
				{
					m_componentList.erase(iter);
					return true;
				}
			}

			return false;
		}


		int64_t m_id;
		int64_t m_parentId;
		std::string m_name;
		std::vector<Component*> m_componentList;

		RTTR_ENABLE()
	private:

		unsigned char m_layer;//将物体分不同的层，用于相机分层、物理碰撞分层等。

		Scene* m_scene;//场景

		bool m_active = true;//是否激活
	};
}
