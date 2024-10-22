﻿
#include "Runtime/Core/pch.h"
#include "CharacterController.h"

#include "rttr/registration"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include "Runtime/Function/Physics/physics.h"
namespace LitchiRuntime
{
    CharacterController::~CharacterController()
    {
        if(m_controller != nullptr)
        {
            Physics::ReleaseController(m_controller);

            m_controller = nullptr;
        }
    }

    void CharacterController::PostResourceModify()
    {
        Component::PostResourceLoaded();
    }

    void CharacterController::OnAwake()
    {
        if(m_controller == nullptr)
        {
            CreatePhysicMaterial();
            auto transform = GetGameObject()->GetTransform();
            m_controller = Physics::CreateDefaultCapsuleController(transform->GetPosition(), m_pxMaterial, Vector3::Zero, Quaternion::Identity, m_radius, m_height / 2.0f);
        }
    }

    Vector3 CharacterController::GetVelocity() const
    {
        return Physics::GetLinearVelocity(dynamic_cast<PxRigidBody*>(Physics::GetControllerRigidActor(m_controller)));
    }

    bool CharacterController::IsGrounded() const
    {
        return (static_cast<int>(m_lastFlags) & static_cast<int>(CollisionFlags::Below)) != 0;
    }

    CharacterController::CollisionFlags CharacterController::GetFlags() const
    {
        return m_lastFlags;
    }

    CharacterController::CollisionFlags CharacterController::SimpleMove(const Vector3& speed)
    {
        auto deltaTime = Time::GetDeltaTime();
        Vector3 displacement = speed;
        displacement += Physics::GetGravity() * deltaTime;
        displacement *= deltaTime;

        return Move(displacement);
    }

    CharacterController::CollisionFlags CharacterController::Move(const Vector3& displacement)
    {
        CollisionFlags result = CollisionFlags::None;
        auto deltaTime = Time::GetDeltaTime();
        if(m_controller != nullptr)
        {
            result = (CollisionFlags)Physics::MoveController(m_controller, displacement,m_minMoveDistance, deltaTime);
            auto position = Physics::GetControllerPosition(m_controller);

            m_lastFlags = result;

            auto transform = GetGameObject()->GetTransform();
            transform->SetPosition(position);
        }

        return result;
    }

    void CharacterController::CreatePhysicMaterial() {

        if (m_pxMaterial == nullptr) {
            m_pxMaterial = Physics::CreateMaterial(m_physicMaterial.GetStaticFriction(), m_physicMaterial.GetDynamicFriction(), m_physicMaterial.GetRestitution());
            m_pxMaterial->setRestitutionCombineMode(PxCombineMode::eAVERAGE);
        }
    }

}