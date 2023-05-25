
#include "game_object.h"
#include "rttr/registration"
#include "Runtime/Core/Log/debug.h"
#include "Runtime/Function/Scene/scene_manager.h"

using namespace rttr;
namespace LitchiRuntime
{
	GameObject::GameObject(std::string name, Scene* scene) : Tree::Node(), layer_(0x01) {
		set_name(name);
		scene->AddGameObject(this);
		this->SetScene(scene);
	}

	GameObject::~GameObject() {
		DEBUG_LOG_INFO("GameObject::~GameObject");
	}


	void GameObject::PostLoadFromAsset()
	{
		// TODO: 从资产中加载的后期加载

	}

	bool GameObject::SetParent(GameObject* parent) {
		if (parent == nullptr) {
			DEBUG_LOG_ERROR("parent null");
			return false;
		}
		parent->AddChild(this);
		return true;
	}

	Scene* GameObject::GetScene()
	{
		return scene_;
	}

	void GameObject::SetScene(Scene* scene)
	{
		scene_ = scene;
	}

	/// 遍历组件
	/// \param func
	void GameObject::ForeachComponent(std::function<void(Component*)> func) {
		for (auto& v : component_list_) {
			
			func(v);
		}
	}

}