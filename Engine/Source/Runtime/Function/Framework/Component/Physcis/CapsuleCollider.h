
#pragma once

#include "rttr/registration"
#include "collider.h"

using namespace rttr;
namespace LitchiRuntime
{
	/**
	 * @brief CapsuleCollider Component
	*/
	class CapsuleCollider : public Collider {
	public:

		/**
		 * @brief Default Constructor
		*/
		CapsuleCollider();

		/**
		 * @brief Default Destructor
		*/
		~CapsuleCollider() override;

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

	protected:

		/**
		 * @brief Create PhysX Shape
		*/
		void CreateShape() override;

		/**
		 * @brief Update Collider shape sphere size
		 * @param radius Current collider radius size
		 * @param height Current collider height size
		*/
		void UpdateShapeRadiusHeight(float radius,float height);


	private:

		/**
		 * @brief Radius of capsule collision body
		*/
		float m_radius;

		/**
		 * @brief Height of capsule collision body
		*/
		float m_height;

		RTTR_ENABLE(Collider);
	};
}
