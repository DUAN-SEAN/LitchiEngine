
#include "rigid_dynamic.h"
#include "rttr/registration"
#include "Runtime/Core/Log/debug.h"
#include "collider.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include "Runtime/Function/Physics/physics.h"
namespace LitchiRuntime
{
    RigidDynamic::RigidDynamic() :enable_ccd_(false) {

    }

    RigidDynamic::~RigidDynamic() {

    }

    void RigidDynamic::UpdateCCDState() {
        PxRigidDynamic* px_rigid_dynamic = dynamic_cast<PxRigidDynamic*>(px_rigid_actor_);
        //~en enable continuous collision detection due to high-speed motion.
        //~zh 对高速运动，开启连续碰撞检测。
        px_rigid_dynamic->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, enable_ccd_);
    }

    void RigidDynamic::Awake() {
        Transform* transform = game_object()->GetComponent<Transform>();
        PxRigidDynamic* px_rigid_dynamic = Physics::CreateRigidDynamic(transform->GetLocalPosition(), game_object()->name().c_str());
        px_rigid_actor_ = dynamic_cast<PxRigidActor*>(px_rigid_dynamic);
        RigidActor::Awake();
    }

    void RigidDynamic::Update() {

    }

    void RigidDynamic::FixedUpdate() {
        if (px_rigid_actor_ == nullptr) {
            DEBUG_LOG_ERROR("px_rigid_actor_== nullptr");
            return;
        }
        //PxRigidBody受Physx物理模拟驱动，位置被改变。获取最新的位置，去更新Transform。
        PxTransform px_transform = px_rigid_actor_->getGlobalPose();
        Transform* transform = game_object()->GetComponent<Transform>();
        transform->SetLocalPosition(glm::vec3(px_transform.p.x, px_transform.p.y, px_transform.p.z));
    }
}