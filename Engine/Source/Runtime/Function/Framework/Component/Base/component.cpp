
#include "component.h"
#include "Runtime/Function/Framework/GameObject/game_object.h"

//注册反射
RTTR_REGISTRATION
{
    registration::class_<Component>("Component")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

Component::Component() {

}

Component::~Component() {

}

void Component::OnEnable() {
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
