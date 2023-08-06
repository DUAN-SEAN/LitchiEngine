
#include "GameObject.h"

#include "rttr/registration"
#include "Runtime/Core/Log/debug.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include "Runtime/Function/Scene/SceneManager.h"
#include "Runtime/Function/Scripting/ScriptEngine.h"

using namespace rttr;
namespace LitchiRuntime
{
	GameObject::GameObject(std::string name,int64_t id) : Tree::Node(), m_layer(0x01),m_id(id), m_parentId(0){
		SetName(name);
	}

	GameObject::~GameObject() {
		DEBUG_LOG_INFO("GameObject::~GameObject");
	}

	bool GameObject::SetParent(GameObject* parent) {

		auto& tran = GetComponent<Transform>()->GetTransform();
		tran.RemoveParent();
		m_parentId = 0;

		if (parent == nullptr) {
			DEBUG_LOG_INFO("parent null");
			return true;
		}

		m_parentId = parent->m_id;
		parent->AddChildNode(this);
		auto& tranParent = parent->GetComponent<Transform>()->GetTransform();
		tran.SetParent(tranParent);

		return true;
	}

	bool GameObject::HasParent()
	{
		auto* parent = GetParentNode();
		auto* root = m_scene->game_object_tree().GetRootNode();
		return parent != nullptr && parent != root;
	}

	void GameObject::PostResourceLoaded()
	{
		// 创建非托管对象
		m_unmanagedId = ScriptEngine::CreateGameObject(m_scene->GetUnmanagedId());

		// 重置parent
		SetParentNode(nullptr);

		for (auto comp : m_componentList)
		{
			// 设置comp的go
			comp->SetGameObject(this);

			// comp执行资源加载后处理
			comp->PostResourceLoaded();
		}
	}

	Scene* GameObject::GetScene()
	{
		return m_scene;
	}

	void GameObject::SetScene(Scene* scene)
	{
		m_scene = scene;
	}

	/// 遍历组件
	/// \param func
	void GameObject::ForeachComponent(std::function<void(Component*)> func) {
		for (auto& v : m_componentList) {
			
			func(v);
		}
	}

}