
#pragma once

#include "Runtime/Core/Math/Vector2.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"

using namespace LitchiRuntime;

namespace LitchiEditor
{
	enum class EGizmoOperation
	{
		TRANSLATE,
		ROTATE,
		SCALE
	};

	/* Handle gizmo behaviours */
	class GizmoBehaviour
	{
	public:
		enum class EDirection
		{
			X,
			Y,
			Z
		};
		
		/**
		* Returns true if the snapping behaviour is enabled
		*/
		bool IsSnappedBehaviourEnabled() const;

		/**
		* Starts the gizmo picking behaviour for the given target in the given direction
		* @param p_actor
		* @param p_cameraPosition
		* @param p_operation
		* @param p_direction
		*/
		void StartPicking(LitchiRuntime::GameObject* p_target, const LitchiRuntime::Vector3& p_cameraPosition, EGizmoOperation p_operation, EDirection p_direction);

		/**
		* Stops the gizmo picking behaviour
		*/
		void StopPicking();

		/**
		* Handle the current behaviour
		* @param p_viewMatrix
		* @param p_projectionMatrix
		* @param p_viewSize
		*/
		void ApplyOperation(const LitchiRuntime::Matrix& p_viewMatrix, const LitchiRuntime::Matrix& p_projectionMatrix, const LitchiRuntime::Vector2& p_viewSize);

		/**
		* Set the given mouse position as the current mouse position and update the previous mouse position
		* @param p_mousePosition
		*/
		void SetCurrentMouse(const LitchiRuntime::Vector2& p_mousePosition);

		/**
		* Returns true if the gizmo is currently picked
		*/
		bool IsPicking() const;

		/**
		* Returns the current operation direction
		*/
		EDirection GetDirection() const;

	private:
		/**
		* Returns the global direction matching with the current m_direction
		*/
		Vector3 GetFakeDirection() const;

		/**
		* Returns the actual direction of the target matching with the current m_direction
		* @param p_relative (If true, the direction depends on hierarchy)
		*/
		Vector3 GetRealDirection(bool p_relative = false) const;

		/**
		* Returns the 3D vector of the arrow projected to the screen
		* @param p_viewMatrix
		* @param p_projectionMatrix
		* @param p_viewSize
		*/
		Vector2 GetScreenDirection(const Matrix& p_viewMatrix, const Matrix& p_projectionMatrix, const Vector2& p_viewSize) const;

		/**
		* Handle the translation behaviour
		* @param p_viewMatrix
		* @param p_projectionMatrix
		* @param p_viewSize
		*/
		void ApplyTranslation(const Matrix& p_viewMatrix, const Matrix& p_projectionMatrix, const Vector2& p_viewSize) const;

		/**
		* Handle the rotation behaviour
		* @param p_viewMatrix
		* @param p_projectionMatrix
		* @param p_viewSize
		*/
		void ApplyRotation(const Matrix& p_viewMatrix, const Matrix& p_projectionMatrix, const Vector2& p_viewSize) const;

		/**
		* Handle the scale behaviour
		* @param p_viewMatrix
		* @param p_projectionMatrix
		* @param p_viewSize
		*/
		void ApplyScale(const Matrix& p_viewMatrix, const Matrix& p_projectionMatrix, const Vector2& p_viewSize) const;

	private:
		bool m_firstMouse = true;
		float m_distanceToActor = 0.0f;
		LitchiRuntime::GameObject* m_target = nullptr;
		EGizmoOperation m_currentOperation;
		EDirection m_direction;
		LitchiRuntime::Transform* m_originalTransform;
		Vector2 m_originMouse;
		Vector2 m_currentMouse;
		Vector2 m_screenDirection;
	};
}
