
#pragma once

#include <queue>

#include "Editor/include/Panels/AView.h"
#include "Runtime/Core/Window/Inputs/InputManager.h"

namespace LitchiRuntime { class RenderCamera; }
namespace LitchiRuntime { class Transform; }
namespace LitchiRuntime { class GameObject; }
namespace LitchiEditor
{
	/**
	* A simple camera controller used to navigate into views
	*/
	class CameraController
	{
	public:
		/**
		* Constructor
		* @param p_view
		* @param p_camera
		* @param p_position
		* @param p_rotation
		* @param p_enableFocusInputs
		*/
		CameraController
		(
			AView& p_view,
			RenderCamera* p_camera,
			glm::vec3& p_position,
			glm::quat& p_rotation,
			bool p_enableFocusInputs = false
		);

		/**
		* Handle mouse and keyboard inputs
		* @parma p_deltaTime
		*/
		void HandleInputs(float p_deltaTime);

		/**
		* Asks the camera to move to the target actor
		* @param p_target
		*/
		void MoveToTarget(GameObject* p_target);

		/**
		* Defines the speed of the camera
		* @param p_speed
		*/
		void SetSpeed(float p_speed);

		/**
		* Returns the camera speed
		*/
		float GetSpeed() const;

		/**
		* Defines the position of the camera
		* @param p_position
		*/
		void SetPosition(const glm::vec3& p_position);

		/**
		* Defines the rotation of the camera
		* @param p_rotation
		*/
		void SetRotation(const glm::quat& p_rotation);

		/**
		* Returns the position of the camera
		*/
		const glm::vec3& GetPosition() const;

		/**
		* Returns the position of the camera
		*/
		const glm::quat& GetRotation() const;

		/**
		* Returns true if the right mouse click is being pressed
		*/
		bool IsRightMousePressed() const;

	private:
		void HandleCameraPanning(const glm::vec2& p_mouseOffset, bool p_firstMouse);
		void HandleCameraOrbit(const glm::vec2& p_mouseOffset, bool p_firstMouse);
		void HandleCameraFPSMouse(const glm::vec2& p_mouseOffset, bool p_firstMouse);

		void HandleCameraZoom();
		void HandleCameraFPSKeyboard(float p_deltaTime);
		void UpdateMouseState();

	private:
		InputManager& m_inputManager;
		Window& m_window;
		AView& m_view;
		RenderCamera* m_camera;
		glm::vec3& m_cameraPosition;
		glm::quat& m_cameraRotation;

		std::queue<std::tuple<glm::vec3, glm::quat>> m_cameraDestinations;

		bool m_enableFocusInputs;

		bool m_leftMousePressed = false;
		bool m_middleMousePressed = false;
		bool m_rightMousePressed = false;

		glm::vec3 m_targetSpeed;
		glm::vec3 m_currentMovementSpeed;

		Transform* m_orbitTarget = nullptr;
		glm::vec3 m_orbitStartOffset;
		bool m_firstMouse = true;
		double m_lastMousePosX = 0.0;
		double m_lastMousePosY = 0.0;
		glm::vec3 m_ypr;
		float m_mouseSensitivity = 0.12f;
		float m_cameraDragSpeed = 0.03f;
		float m_cameraOrbitSpeed = 0.5f;
		float m_cameraMoveSpeed = 15.0f;
		float m_focusDistance = 15.0f;
		float m_focusLerpCoefficient = 8.0f;
	};
}
