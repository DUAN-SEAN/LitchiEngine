
#pragma once

#include "rttr/registration"
#include "collider.h"

using namespace rttr;
namespace LitchiRuntime
{
	/**
	 * @brief SphereCollider Component
	*/
	class SphereCollider : public Collider {
	public:

		/**
		 * @brief Default Constructor
		*/
		SphereCollider();

		/**
		 * @brief Default Destructor
		*/
		~SphereCollider() override;

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
		 * @param radius Current collider box size
		*/
		void UpdateShapeRadius(float radius);


	private:

		/**
		 * @brief Radius of sphere collision body
		*/
		float m_radius;

		RTTR_ENABLE(Collider);
	};
}
