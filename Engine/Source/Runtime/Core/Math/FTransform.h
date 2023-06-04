
#pragma once

#include "glm.hpp"
#include "TransformNotifier.h"
#include "gtc/quaternion.hpp"

namespace LitchiRuntime
{
	/**
	* Mathematic representation of a 3D FTransformation with float precision
	*/
	class FTransform
	{
	public:
		/**
		* Create a FTransform without setting a parent
		* @param p_localPosition
		* @param p_localRotation
		* @param p_localScale
		*/
		FTransform(glm::vec3 p_localPosition = glm::vec3(0.0f, 0.0f, 0.0f), glm::quat p_localRotation = glm::quat(1,0,0,0), glm::vec3 p_localScale = glm::vec3(1.0f, 1.0f, 1.0f));

		/**
		* Destructor of the FTransform.
		* Will notify children on destruction
		*/
		~FTransform();

		/**
		* Simple callback that will treat parent notifications
		* @param p_notification
		*/
		void NotificationHandler(TransformNotifier::ENotification p_notification);

		/**
		* Defines a parent to the FTransform
		* @param p_parent
		*/
		void SetParent(FTransform& p_parent);

		/**
		* Set the parent to nullptr and recalculate world matrix
		* Returns true on success
		*/
		bool RemoveParent();

		/**
		* Check if the FTransform has a parent
		*/
		bool HasParent() const;

		/**
		* Initialize FTransform with raw data
		* @param p_position
		* @param p_rotation
		* @param p_scale
		*/
		void GenerateMatrices(glm::vec3 p_position, glm::quat p_rotation, glm::vec3 p_scale);

		/**
		* Re-update world matrix to use parent FTransformations
		*/
		void UpdateWorldMatrix();

		/**
		* Set the position of the FTransform in the local space
		* @param p_newPosition
		*/
		void SetLocalPosition(glm::vec3 p_newPosition);

		/**
		* Set the rotation of the FTransform in the local space
		* @param p_newRotation
		*/
		void SetLocalRotation(glm::quat p_newRotation);

		/**
		* Set the scale of the FTransform in the local space
		* @param p_newScale
		*/
		void SetLocalScale(glm::vec3 p_newScale);

		/**
		* Set the position of the FTransform in world space
		* @param p_newPosition
		*/
		void SetWorldPosition(glm::vec3 p_newPosition);

		/**
		* Set the rotation of the FTransform in world space
		* @param p_newRotation
		*/
		void SetWorldRotation(glm::quat p_newRotation);

		/**
		* Set the scale of the FTransform in world space
		* @param p_newScale
		*/
		void SetWorldScale(glm::vec3 p_newScale);

		/**
		* Translate in the local space
		* @param p_translation
		*/
		void TranslateLocal(const glm::vec3& p_translation);

		/**
		* Rotate in the local space
		* @param p_rotation
		*/
		void RotateLocal(const glm::quat& p_rotation);

		/**
		* Scale in the local space
		* @param p_scale
		*/
		void ScaleLocal(const glm::vec3& p_scale);

		/**
		* Return the position in local space
		*/
		const glm::vec3& GetLocalPosition() const;

		/**
		* Return the rotation in local space
		*/
		const glm::quat& GetLocalRotation() const;

		/**
		* Return the scale in local space
		*/
		const glm::vec3& GetLocalScale() const;

		/**
		* Return the position in world space
		*/
		const glm::vec3& GetWorldPosition() const;

		/**
		* Return the rotation in world space
		*/
		const glm::quat& GetWorldRotation() const;

		/**
		* Return the scale in world space
		*/
		const glm::vec3& GetWorldScale() const;

		/**
		* Return the local matrix
		*/
		const glm::mat4& GetLocalMatrix() const;

		/**
		* Return the world matrix
		*/
		const glm::mat4& GetWorldMatrix() const;

		/**
		* Return the FTransform world forward
		*/
		glm::vec3 GetWorldForward() const;

		/**
		* Return the FTransform world up
		*/
		glm::vec3 GetWorldUp() const;

		/**
		* Return the FTransform world right
		*/
		glm::vec3 GetWorldRight() const;

		/**
		* Return the FTransform local forward
		*/
		glm::vec3 GetLocalForward() const;

		/**
		* Return the FTransform local up
		*/
		glm::vec3 GetLocalUp() const;

		/**
		* Return the FTransform local right
		*/
		glm::vec3 GetLocalRight() const;

	public:
		TransformNotifier Notifier;
		TransformNotifier::NotificationHandlerID m_notificationHandlerID;

	private:
		void PreDecomposeWorldMatrix();
		void PreDecomposeLocalMatrix();

		/* Pre-decomposed data to prevent multiple decomposition */
		glm::vec3 m_localPosition{};
		glm::quat m_localRotation{};
		glm::vec3 m_localScale{};
		glm::vec3 m_worldPosition{};
		glm::quat m_worldRotation{};
		glm::vec3 m_worldScale{};

		glm::mat4 m_localMatrix{};
		glm::mat4 m_worldMatrix{};

		FTransform*	m_parent;
	};
}
