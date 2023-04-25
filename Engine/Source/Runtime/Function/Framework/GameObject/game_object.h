﻿
#ifndef UNTITLED_GAME_OBJECT_H
#define UNTITLED_GAME_OBJECT_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <list>
#include <functional>
#include "Runtime/Core/DataStruct/tree.h"
#include "Runtime/Function/Framework/Component/Base/component.h"

namespace LitchiRuntime
{
	class GameObject :public Tree::Node {
	public:
		GameObject(){}
		GameObject(std::string name);
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

		/// 全局查找GameObject
		/// \param name
		/// \return
		static GameObject* Find(const char* name);
	public:
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

			if (components_map_.find(component_type_name) == components_map_.end()) {
				std::vector<Component*> component_vec;
				component_vec.push_back(component);
				components_map_[component_type_name] = component_vec;
			}
			else {
				components_map_[component_type_name].push_back(component);
			}
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

			if (components_map_.find(component_type_name) != components_map_.end()) {
				component_vec = components_map_[component_type_name];
			}
			if (component_vec.size() == 0) {
				//没有找到组件,就去查找子类组件
				auto derived_classes = t.get_derived_classes();
				for (auto derived_class : derived_classes) {
					std::string derived_class_type_name = derived_class.get_name().to_string();
					if (components_map_.find(derived_class_type_name) != components_map_.end()) {
						component_vec = components_map_[derived_class_type_name];
						if (component_vec.size() != 0) {
							break;
						}
					}
				}
			}
			if (component_vec.size() == 0) {
				return nullptr;
			}
			return dynamic_cast<T*>(component_vec[0]);
		}

		/// 遍历组件
		/// \param func
		void ForeachComponent(std::function<void(Component*)> func);

		std::string name_;
		std::unordered_map<std::string, std::vector<Component*>> components_map_;

		/// 遍历GameObject
		/// \param func
		static void Foreach(std::function<void(GameObject* game_object)> func);
	private:

		unsigned char layer_;//将物体分不同的层，用于相机分层、物理碰撞分层等。

		bool active_ = true;//是否激活


		static Tree game_object_tree_;//用树存储所有的GameObject。

		static std::list<GameObject*> game_object_list_;//存储所有的GameObject。
	};
}

#endif //UNTITLED_GAME_OBJECT_H
