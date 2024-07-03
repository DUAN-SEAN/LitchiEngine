#include "Runtime/Core/pch.h"
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

    void RigidDynamic::OnAwake() {
        Transform* transform = GetGameObject()->GetComponent<Transform>();
        PxRigidDynamic* px_rigid_dynamic = Physics::CreateRigidDynamic(transform->GetPosition(),transform->GetRotation(), GetGameObject()->GetName().c_str());
        m_pxRigidActor = dynamic_cast<PxRigidActor*>(px_rigid_dynamic);
        RigidActor::OnAwake();
    }

    void RigidDynamic::OnUpdate() {

        auto transform = GetGameObject()->GetComponent<Transform>();
        if(m_pxRigidActor != nullptr && transform != nullptr)
        {
            auto globalPose = m_pxRigidActor->getGlobalPose();
            auto globalPosition = Vector3(globalPose.p.x, globalPose.p.y, globalPose.p.z);
            auto globalRotation = Quaternion(globalPose.q.x, globalPose.q.y, globalPose.q.z, globalPose.q.w);

            auto& gameObjectWorldPos = transform->GetPosition();
            auto& gameObjectWorldRotation = transform->GetRotation();

            if(gameObjectWorldPos != globalPosition || gameObjectWorldRotation != globalRotation)
            {
                m_pxRigidActor->setGlobalPose(PxTransform(PxVec3(gameObjectWorldPos.x,gameObjectWorldPos.y,gameObjectWorldPos.z),PxQuat(gameObjectWorldRotation.x, gameObjectWorldRotation.y, gameObjectWorldRotation.z, gameObjectWorldRotation.w)));
            }
        }
    }

    void RigidDynamic::OnFixedUpdate() {
        if (m_pxRigidActor == nullptr) {
            DEBUG_LOG_ERROR("px_rigid_actor_== nullptr");
            return;
        }

        PxTransform px_transform = m_pxRigidActor->getGlobalPose();
        Transform* transform = GetGameObject()->GetComponent<Transform>();
        transform->SetPosition(Vector3(px_transform.p.x, px_transform.p.y, px_transform.p.z));
        transform->SetRotation(Quaternion(px_transform.q.x, px_transform.q.y, px_transform.q.z, px_transform.q.w));
    }
}