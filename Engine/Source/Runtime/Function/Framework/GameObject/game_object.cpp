
#include "game_object.h"
#include "rttr/registration"
#include "Runtime/Core/Log/debug.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include "Runtime/Function/Scene/scene_manager.h"

using namespace rttr;
namespace LitchiRuntime
{
	GameObject::GameObject(std::string name,int64_t id) : Tree::Node(), layer_(0x01),id_(id), parentId_(0){
		set_name(name);
	}

	GameObject::~GameObject() {
		DEBUG_LOG_INFO("GameObject::~GameObject");
	}

	bool GameObject::SetParent(GameObject* parent) {

		auto& tran = GetComponent<Transform>()->GetTransform();
		tran.RemoveParent();
		parentId_ = 0;

		if (parent == nullptr) {
			DEBUG_LOG_INFO("parent null");
			return true;
		}

		parentId_ = parent->parentId_;
		parent->AddChild(this);
		auto& tranParent = parent->GetComponent<Transform>()->GetTransform();
		tran.SetParent(tranParent);

		return true;
	}

	void GameObject::PostResourceLoaded()
	{
		// 重置parent
		set_parent(nullptr);

		for (auto comp : component_list_)
		{
			// 设置comp的go
			comp->set_game_object(this);

			// comp执行资源加载后处理
			comp->PostResourceLoaded();
		}
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