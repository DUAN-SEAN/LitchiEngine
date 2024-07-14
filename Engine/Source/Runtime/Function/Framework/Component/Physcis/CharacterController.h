
#pragma once

#include "rttr/registration"
#include "collider.h"

using namespace rttr;
namespace LitchiRuntime
{
	/**
	 * @brief CharacterController Component
	*/
	class CharacterController : public Component {
	public:

		/**
		 * @brief Default Constructor
		*/
		CharacterController();

		/**
		 * @brief Default Destructor
		*/
		~CharacterController() override;

		/**
		 * @brief Set SphereCollider Size
		 * @param radius
		*/
		void SetRadius(float radius)
		{
			m_radius = radius;
		}

		/**
		 * @brief Get SphereCollider Size
		 * @return
		*/
		float GetRadius()
		{
			return m_radius;
		}

		/**
		 * @brief Set SphereCollider Size
		 * @param height
		*/
		void SetHeight(float height)
		{
			m_height = height;
		}

		/**
		 * @brief Get SphereCollider Size
		 * @return
		*/
		float GetHeight()
		{
			return m_height;
		}

		/**
		 * @brief Call before object resource change
		*/
		void PostResourceModify() override;

	private:

		/**
		 * @brief Radius of capsule collision body
		*/
		float m_radius;

		/**
		 * @brief Height of capsule collision body
		*/
		float m_height;

		/**
		 * \brief PhysX Controller Handle
		 */
		PxController* m_controller;

		RTTR_ENABLE(Component);
	};
}
