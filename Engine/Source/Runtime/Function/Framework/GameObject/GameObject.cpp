
#include "GameObject.h"

#include "rttr/registration"
#include "Runtime/Core/Log/debug.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include "Runtime/Function/Scene/SceneManager.h"
#include "Runtime/Function/Scripting/ScriptEngine.h"

using namespace rttr;
namespace LitchiRuntime
{
	GameObject::GameObject(std::string name,int64_t id) : m_layer(0x01),m_id(id), m_parentId(0){
		SetName(name);

		Initialize();
	}

	GameObject::~GameObject() {
		DEBUG_LOG_INFO("GameObject::~GameObject");

		UnInitialize();
	}

	void GameObject::Initialize()
	{
		AddComponent<Transform>();
	}

	void GameObject::SetActive(bool active)
	{
		m_active = active;
		m_scene->Resolve();
	}

	bool GameObject::SetParent(GameObject* parent) {

		if(parent)
		{
			GetComponent<Transform>()->SetParent(parent->GetComponent<Transform>());
		}else
		{
			GetComponent<Transform>()->SetParent(nullptr);
		}

		return true;
	}

	bool GameObject::HasParent()
	{
		return GetComponent<Transform>()->HasParent();
	}
	GameObject* GameObject::GetParent()
	{
		return GetComponent<Transform>()->GetParent()->GetGameObject();
	}

	std::list<GameObject*>& GameObject::GetChildren()
	{
		std::list<GameObject*> dadList;
		for (auto a : GetComponent<Transform>()->GetChildren()) {
			dadList.push_back(a->GetGameObject());       //依次加到父类List里
		}

		return dadList;
	}
	void GameObject::PostResourceLoaded()
	{
		// 创建非托管对象
		m_unmanagedId = ScriptEngine::CreateGameObject(m_scene->GetUnmanagedId());

		// 重置parent
		SetParent(nullptr);

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

	void GameObject::UnInitialize()
	{
		auto tempList = m_componentList;
		for (auto& v : tempList) {
			RemoveComponent(v);
		}
	}

}