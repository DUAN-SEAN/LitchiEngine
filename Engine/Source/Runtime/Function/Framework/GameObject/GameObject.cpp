
#include "GameObject.h"

#include "rttr/registration"
#include "Runtime/Core/Log/debug.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include "Runtime/Function/Scene/SceneManager.h"
#include "Runtime/Function/Scripting/ScriptEngine.h"

using namespace rttr;
namespace LitchiRuntime
{
	GameObject::GameObject(const std::string& name, int64_t& id, bool& isPlaying) :
		m_layer(0x01),
		m_id{ id },
		m_parentId{0},
		m_isPlaying{ isPlaying }
	{
		SetName(name);

		Initialize();
	}

	GameObject::~GameObject() {
		DEBUG_LOG_INFO("GameObject::~GameObject");

		UnInitialize();
	}

	void GameObject::Initialize()
	{
	}

	void GameObject::SetActive(bool active)
	{
		if(m_active != active)
		{
			RecursiveWasActiveUpdate();
			m_active = active;
			RecursiveActiveUpdate();

			if (m_scene)
			{
				m_scene->Resolve();
			}
			
		}

	}

	bool GameObject::SetParent(GameObject* parent)
	{
		// remove old parent
		auto oldParent = GetComponent<Transform>()->GetParent();
		if(oldParent)
		{
			// m_parentId = 0;
			oldParent->RemoveChild(GetComponent<Transform>());
		}

		// add new parent
		if(parent)
		{
			m_parentId = parent->m_id;
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

	std::list<GameObject*> GameObject::GetChildren()
	{
		std::list<GameObject*> dadList;
		for (auto a : GetComponent<Transform>()->GetChildren()) {
			dadList.push_back(a->GetGameObject());       //依次加到父类List里
		}

		return dadList;
	}
	void GameObject::PostResourceLoaded()
	{
		//// 创建非托管对象
		//m_unmanagedId = ScriptEngine::CreateGameObject(m_scene->GetUnmanagedId());

		// 重置parent
		SetParent(nullptr);

		for (auto comp : m_componentList)
		{
			// 设置comp的go
			comp->SetGameObject(this);

			// comp执行资源加载后处理
			comp->PostResourceLoaded();
			
			if (m_isPlaying && GetActive())
			{
				comp->OnAwake();
				comp->OnEnable();
				comp->OnStart();
			}
		}
	}

	void GameObject::SetSleeping(bool p_sleeping)
	{
		m_sleeping = p_sleeping;
	}

	void GameObject::OnAwake()
	{
		m_awaked = true;
		std::for_each(m_componentList.begin(), m_componentList.end(), [](auto* element) { element->OnAwake(); });

		// todo script OnWake
	}

	void GameObject::OnEnable()
	{
		std::for_each(m_componentList.begin(), m_componentList.end(), [](auto* element) { element->OnEnable(); });
	}

	void GameObject::OnStart()
	{
		std::for_each(m_componentList.begin(), m_componentList.end(), [](auto* element) { element->OnStart(); });
	}

	void GameObject::OnDisable()
	{
		std::for_each(m_componentList.begin(), m_componentList.end(), [](auto* element) { element->OnDisable(); });
	}

	void GameObject::OnDestroy()
	{
		std::for_each(m_componentList.begin(), m_componentList.end(), [](auto* element) { element->OnDestroy(); });
	}

	void GameObject::OnUpdate()
	{
		if(GetActive())
		{
			std::for_each(m_componentList.begin(), m_componentList.end(), [](auto* element) { element->OnUpdate(); });
		}
	}

	void GameObject::OnFixedUpdate()
	{
		if (GetActive())
		{
			std::for_each(m_componentList.begin(), m_componentList.end(), [](auto* element) { element->OnFixedUpdate(); });
		}
	}

	void GameObject::OnLateUpdate()
	{
		if (GetActive())
		{
			std::for_each(m_componentList.begin(), m_componentList.end(), [](auto* element) { element->OnLateUpdate(); });
		}
	}

	void GameObject::OnCollisionEnter(Collider* p_otherObject)
	{
		// do nothing
	}

	void GameObject::OnCollisionStay(Collider* p_otherObject)
	{
	}

	void GameObject::OnCollisionExit(Collider* p_otherObject)
	{
	}

	void GameObject::OnTriggerEnter(Collider* p_otherObject)
	{
	}

	void GameObject::OnTriggerStay(Collider* p_otherObject)
	{
	}

	void GameObject::OnTriggerExit(Collider* p_otherObject)
	{
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

	void GameObject::RecursiveActiveUpdate()
	{
		bool isActive = GetActive();

		if (!m_sleeping)
		{
			if (!m_wasActive && isActive)
			{
				if (!m_awaked)
					OnAwake();

				OnEnable();

				if (!m_started)
					OnStart();
			}

			if (m_wasActive && !isActive)
				OnDisable();
		}

		for (auto child : GetChildren())
			child->RecursiveActiveUpdate();
	}

	void GameObject::RecursiveWasActiveUpdate()
	{
		m_wasActive = GetActive();

		for (auto child : GetChildren())
			child->RecursiveWasActiveUpdate();
	}

}