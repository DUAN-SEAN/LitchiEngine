
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
		  * \brief  Specifies which sides a character is colliding with.
		  */
		enum class CollisionFlags
		{

			/**
			 * \brief The character is not colliding.
			 */
			None = 0,

			/**
			 * \brief The character is colliding to the sides.
			 */
			Sides = 1 << 0,

			/**
			 * \brief The character has collision above.
			 */
			Above = 1 << 1,

			/**
			 * \brief The character has collision below.
			 */
			Below = 1 << 2,

		};
		/**
		 * @brief Default Constructor
		*/
		CharacterController() = default;

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
		 * @brief Set PhysicMaterial
		 * @param physicMaterialRes
		*/
		void SetPhysicMaterial(PhysicMaterialRes physicMaterialRes)
		{
			m_physicMaterial = physicMaterialRes;
		}

		/**
		 * @brief Get PhysicMaterial
		 * @return
		*/
		PhysicMaterialRes GetPhysicMaterial()
		{
			return m_physicMaterial;
		}

		/**
		 * \brief Gets the step height. The character will step up a stair only if it is closer to the ground than the indicated value. This should not be greater than the Character Controller’s height or it will generate an error.
		 * \return 
		 */
		float GetStepOffset() const
		{
			return m_stepOffset;
		}

		/**
		 * \brief  Sets the step height. The character will step up a stair only if it is closer to the ground than the indicated value. This should not be greater than the Character Controller’s height or it will generate an error.
		 * \param stepOffset 
		 */
		void SetStepOffset(float stepOffset)
		{
			m_stepOffset = stepOffset;
		}

		/**
		 * \brief Gets the slope limit (in degrees). Limits the collider to only climb slopes that are less steep (in degrees) than the indicated value.
		 * \return 
		 */
		float GetSlopeLimit() const
		{
			return m_slopeLimit;
		}

		/**
		 * \brief  Sets the slope limit (in degrees). Limits the collider to only climb slopes that are less steep (in degrees) than the indicated value.
		 * \param slopeLimit 
		 */
		void SetSlopeLimit(float slopeLimit)
		{
			m_slopeLimit = slopeLimit;
		}

		/**
		 * \brief Gets the minimum move distance of the character controller. The minimum traveled distance to consider. If traveled distance is smaller, the character doesn't move. This is used to stop the recursive motion algorithm when remaining distance to travel is small.
		 * \return 
		 */
		float GetMinMoveDistance() const
		{
			return m_minMoveDistance;
		}

		/**
		 * \brief Sets the minimum move distance of the character controller.The minimum traveled distance to consider. If traveled distance is smaller, the character doesn't move. This is used to stop the recursive motion algorithm when remaining distance to travel is small.
		 * \param minMoveDistance 
		 */
		void SetMinMoveDistance(float minMoveDistance)
		{
			m_minMoveDistance = minMoveDistance;
		}

		/**
		 * \brief  Sets the character up vector.
		 * \return 
		 */
		Vector3 GetUpDirection() const
		{
			return m_upDirection;
		}

		/**
		 * \brief Gets the character up vector.
		 * \param upDirection 
		 */
		void SetUpDirection(Vector3 upDirection)
		{
			m_upDirection = upDirection;
		}

		/**
		 * @brief Call before object resource change
		*/
		void PostResourceModify() override;

		void OnAwake() override;
	public:

		/**
		 * \brief Gets the linear velocity of the Character Controller. This allows tracking how fast the character is actually moving, for instance when it is stuck at a wall this value will be the near zero vector.
		 * \return 
		 */
		Vector3 GetVelocity() const;

		/**
		 * \brief Gets a value indicating whether this character was grounded during last move call grounded.
		 * \return 
		 */
		bool IsGrounded() const;

		/**
		 * \brief Gets the current collision flags. Tells which parts of the character capsule collided with the environment during the last move call. It can be used to trigger various character animations.
		
		 * \return 
		 */
		CollisionFlags GetFlags() const;
	public:

		/**
		 * \brief Moves the character with the given speed. Gravity is automatically applied. It will slide along colliders. Result collision flags is the summary of collisions that occurred during the Move.
		 * \param speed
		 * \return
		 */
		CollisionFlags SimpleMove(const Vector3& speed);

		/**
		 * \brief Moves the character using a 'collide-and-slide' algorithm. Attempts to move the controller by the given displacement vector, the motion will only be constrained by collisions. It will slide along colliders. Result collision flags is the summary of collisions that occurred during the Move. This function does not apply any gravity.
		 * \param displacement
		 * \return
		 */
		CollisionFlags Move(const Vector3& displacement);
	private:

		/**
		 * @brief Create PhysicMaterial
		*/
		void CreatePhysicMaterial();

		/**
		 * \brief PhysX Controller Handle
		 */
		PxController* m_controller = nullptr;

		/**
		 * @brief PhysX PxMaterial pointer
		*/
		PxMaterial* m_pxMaterial = nullptr;

		/**
		 * @brief Radius of capsule collision body. measured in the object's local space. The sphere radius will be scaled by the actor's world scale.
		*/
		float m_radius = 1.0f;

		/**
		 * @brief Height of capsule collision body. measured in the object's local space. The capsule height will be scaled by the actor's world scale.
		*/
		float m_height = 3.0f;

		/**
		 * \brief he character will step up a stair only if it is closer to the ground than the indicated value. This should not be greater than the Character Controller’s height or it will generate an error.
		 */
		float m_stepOffset = 0.6f;

		/**
		 * \brief  Limits the collider to only climb slopes that are less steep (in degrees) than the indicated value.
		 */
		float m_slopeLimit = 45.0f;

		/**
		 * \brief The minimum traveled distance to consider. If traveled distance is smaller, the character doesn't move. This is used to stop the recursive motion algorithm when remaining distance to travel is small.
		 */
		float m_minMoveDistance = 0.0f;

		/**
		 * \brief The character will step up a stair only if it is closer to the ground than the indicated value. This should not be greater than the Character Controller’s height or it will generate an error.
		 */
		Vector3 m_upDirection = Vector3::Up;

		/**
		 * @brief PhysicMaterialRes
		*/
		PhysicMaterialRes m_physicMaterial;

		/**
		 * \brief last move collision flags
		 */
		CollisionFlags m_lastFlags;

		RTTR_ENABLE(Component)
	};
}
