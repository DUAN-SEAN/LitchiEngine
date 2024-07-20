#include "Runtime/Core/pch.h"
#include "ThirdPersonController.h"

#include "rttr/registration"
#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Core/Log/debug.h"
#include "Runtime/Core/Window/Inputs/InputManager.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include "Runtime/Function/Physics/physics.h"

namespace LitchiRuntime
{
	ThirdPersonController::ThirdPersonController() : m_controller(nullptr) {

	}

	ThirdPersonController::~ThirdPersonController() {


	}

	void ThirdPersonController::OnAwake()
	{
		m_controller = GetGameObject()->GetComponent<CharacterController>();
	}

	void ThirdPersonController::OnUpdate()
	{
		if (m_controller)
		{
			auto transform = GetGameObject()->GetTransform();

			auto forwardKeyState = InputManager::GetKeyState(EKey::KEY_W);
			auto backKeyState = InputManager::GetKeyState(EKey::KEY_S);
			auto leftKeyState = InputManager::GetKeyState(EKey::KEY_A);
			auto rightKeyState = InputManager::GetKeyState(EKey::KEY_D);

			Vector3 moveDist = Vector3::Zero;
			if (forwardKeyState == EKeyState::KEY_DOWN)
			{
				moveDist = transform->GetForward()* m_moveLinearSpeed* Time::GetDeltaTime();
			}
			else if (backKeyState == EKeyState::KEY_DOWN)
			{
				moveDist = -transform->GetForward() * m_moveLinearSpeed * Time::GetDeltaTime();
			}

			m_fallVelocity += Physics::GetGravity() * Time::GetDeltaTime();
			moveDist += m_fallVelocity * Time::GetDeltaTime();

			auto flag  = m_controller->Move(moveDist);
			if(flag == CharacterController::CollisionFlags::Below)
			{
				m_fallVelocity = Vector3::Zero;
			}


			Quaternion rotate = Quaternion::Identity;
			if (leftKeyState == EKeyState::KEY_DOWN)
			{
				rotate = Quaternion::FromAngleAxis(Math::Helper::DegreesToRadians(m_rotateSpeed) * Time::GetDeltaTime(), Vector3::Up);
			}
			else if (rightKeyState == EKeyState::KEY_DOWN)
			{
				rotate = Quaternion::FromAngleAxis(-Math::Helper::DegreesToRadians(m_rotateSpeed) * Time::GetDeltaTime(), Vector3::Up);
			}
			transform->Rotate(rotate);
		}
	}


}
