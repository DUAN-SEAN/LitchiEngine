
#ifndef UNTITLED_GAME_OBJECT_H
#define UNTITLED_GAME_OBJECT_H

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

		std::string name() { return name_; }
		void set_name(std::string name) { name_ = name; }

		unsigned char layer() { return layer_; }
		void set_layer(unsigned char layer) { layer_ = layer; }

		bool active() { return active_; }
		void set_active(bool active) { active_ = active; }

		/// 设置父节点
		/// \param parent
		/// \return
		bool SetParent(GameObject* parent);

		bool HasParent() { return parent() != nullptr; }

		GameObject* GetParent() { return (GameObject*)parent(); }

		std::list<GameObject*>& GetChildren()
		{
			std::list<GameObject*> dadList;
			for (auto a : children()) {
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
			component->set_game_object(this);
			//获取类名
			type t = type::get<T>();
			std::string component_type_name = t.get_name().to_string();
			
			component_list_.push_back(component);
			
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

			for (auto iter = component_list_.begin(); iter != component_list_.end(); iter++)
			{
				if ((*iter)->get_type().get_name() == component_type_name)
				{
					return dynamic_cast<T*>(*iter);
				}
			}
			return nullptr;
		}

		 std::vector<Component*>& GetComponents() { return component_list_; }

		/// 遍历组件
		/// \param func
		void ForeachComponent(std::function<void(Component*)> func);
		bool RemoveComponent(Component* component)
		{
			//获取类名
			type t = component->get_type();
			std::string component_type_name = t.get_name().to_string();
			for (auto iter = component_list_.begin(); iter != component_list_.end(); iter++)
			{
				if (*iter == component)
				{
					component_list_.erase(iter);
					return true;
				}
			}

			return false;
		}


		int64_t id_;
		int64_t parentId_;
		std::string name_;
		std::vector<Component*> component_list_;

		RTTR_ENABLE()
	private:

		unsigned char layer_;//将物体分不同的层，用于相机分层、物理碰撞分层等。

		Scene* scene_;//场景

		bool active_ = true;//是否激活
	};
}

#endif //UNTITLED_GAME_OBJECT_H
