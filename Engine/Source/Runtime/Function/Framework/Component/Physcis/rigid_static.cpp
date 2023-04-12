
#include "rigid_static.h"
#include "rttr/registration"
#include "Runtime/Core/Log/debug.h"
#include "collider.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include "Runtime/Function/Physics/physics.h"

using namespace rttr;
RTTR_REGISTRATION{
    registration::class_<RigidStatic>("RigidStatic")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

RigidStatic::RigidStatic() {

}

RigidStatic::~RigidStatic(){

}

void RigidStatic::Awake() {
    Transform* transform=game_object()->GetComponent<Transform>();
    PxRigidStatic* px_rigid_static_=Physics::CreateRigidStatic(transform->position(), game_object()->name());
    px_rigid_actor_=dynamic_cast<PxRigidActor*>(px_rigid_static_);
    RigidActor::Awake();
}

void RigidStatic::Update() {

}

void RigidStatic::FixedUpdate() {
}