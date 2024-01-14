
#include "component.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Scripting/ScriptEngine.h"

namespace LitchiRuntime
{
	Component::Component():ScriptObject() {
	}

	Component::~Component() {

	}

	void Component::OnEnable() {
	}

	void Component::OnCreate()
	{
	}

	void Component::Awake() {
	}

	void Component::Update() {
	}


	void Component::FixedUpdate() {
	}


	void Component::OnPreRender() {
	}

	void Component::OnPostRender() {
	}

	void Component::OnDisable() {
	}

	void Component::OnTriggerEnter(GameObject* game_object) {
	}

	void Component::OnTriggerExit(GameObject* game_object) {
	}

	void Component::OnTriggerStay(GameObject* game_object) {
	}

	void Component::PostResourceLoaded()
	{
	/*	const auto typeName = get_type().get_name().to_string();
		const auto gameObjectUnmanagedId = m_gameObject->GetUnmanagedId();
		m_unmanagedId = ScriptEngine::CreateComponent(gameObjectUnmanagedId, typeName);*/
	}

	void Component::PostResourceModify()
	{
		PostResourceLoaded();
	}
}
