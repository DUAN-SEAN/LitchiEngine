
#include "RigidDynamic.h"

#include "rttr/registration"
#include "Runtime/Core/Log/debug.h"
#include "collider.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Physics/physics.h"

namespace LitchiRuntime
{
    RigidDynamic::RigidDynamic() :m_enableCCD(false) {

    }

    RigidDynamic::~RigidDynamic() {
    }

    void RigidDynamic::UpdateCCDState() {
        PxRigidDynamic* px_rigid_dynamic = dynamic_cast<PxRigidDynamic*>(m_pxRigidActor);
        //~en enable continuous collision detection due to high-speed motion.
        //~zh 对高速运动，开启连续碰撞检测。
        px_rigid_dynamic->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, m_enableCCD);
    }

    void RigidDynamic::Awake() {
        Transform* transform = GetGameObject()->GetComponent<Transform>();
        PxRigidDynamic* px_rigid_dynamic = Physics::CreateRigidDynamic(transform->GetPosition(),transform->GetRotation(), GetGameObject()->GetName().c_str());
        m_pxRigidActor = dynamic_cast<PxRigidActor*>(px_rigid_dynamic);
        RigidActor::Awake();
    }

    void RigidDynamic::Update() {

    }

    void RigidDynamic::FixedUpdate() {
        if (m_pxRigidActor == nullptr) {
            DEBUG_LOG_ERROR("px_rigid_actor_== nullptr");
            return;
        }
        //PxRigidBody受Physx物理模拟驱动，位置被改变。获取最新的位置，去更新Transform。
        PxTransform px_transform = m_pxRigidActor->getGlobalPose();
        Transform* transform = GetGameObject()->GetComponent<Transform>();
        transform->SetPosition(Vector3(px_transform.p.x, px_transform.p.y, px_transform.p.z));
    }
}