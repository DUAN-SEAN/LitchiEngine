#pragma once

#include "Runtime/Function/Framework/Component/Animation/Animator.h"
#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Framework/Component/Camera/camera.h"
#include "Runtime/Function/Framework/Component/Physcis/CharacterController.h"

using namespace physx;
namespace LitchiRuntime
{
	/**
	 * @brief ThirdPersonController Component
	*/
	class ThirdPersonController : public Component {
	public:

		/**
		 * @brief Default Constructor
		*/
		ThirdPersonController();

		/**
		 * @brief Default Destructor
		*/
		~ThirdPersonController() override;

		float GetMoveLinearSpeed()
		{
			return m_moveLinearSpeed;
		}

		void SetMoveLinearSpeed(float moveLinearSpeed)
		{
			m_moveLinearSpeed = moveLinearSpeed;
		}
	
		float GetwRotateSpeed()
		{
			return m_rotateSpeed;
		}

		void SetRotateSpeed(float rotateSpeed)
		{
			m_rotateSpeed = rotateSpeed;
		}

		/**
		 * @brief Called when the scene start right before OnStart
		 * It allows you to apply prioritized game logic on scene start
		*/
		void OnAwake() override;


		/**
		 * \brief OnUpdate
		 */
		void OnUpdate() override;

	protected:


		/**
		 * \brief camera
		 */
		Camera* m_camera;

		/**
		 * \brief character controller
		 */
		CharacterController* m_controller;

		/**
		 * \brief Animator
		 */
		Animator* m_animator;

		/**
		 * \brief linear move speed per second
		 */
		float m_moveLinearSpeed = 2.0f;

		/**
		 * \brief rotation degree speed per second
		 */
		float m_rotateSpeed = 60.0f;

		Vector3 m_fallVelocity = Vector3::Zero;

	public:

		RTTR_ENABLE(Component)
	};
}
