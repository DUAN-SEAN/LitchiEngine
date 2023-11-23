
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
			Vector3& p_position,
			Quaternion& p_rotation,
			bool p_enableFocusInputs = false
		);

		/**
		* Handle mouse and keyboard inputs
		* @parma p_deltaTime
		*/
		void HandleInputs(float p_deltaTime);

		/**
		* Defines the position of the camera
		* @param p_position
		*/
		void SetPosition(const Vector3& p_position);

		/**
		* Defines the rotation of the camera
		* @param p_rotation
		*/
		void SetRotation(const Quaternion& p_rotation);
		void MoveToTarget(GameObject* target);

		/**
		* Returns the position of the camera
		*/
		const Vector3& GetPosition() const;

		/**
		* Returns the position of the camera
		*/
		const Quaternion& GetRotation() const;
		
	private:
		Window& m_window;
		AView& m_view;
		RenderCamera* m_camera;
	/*	Vector3& m_cameraPosition;
		Quaternion& m_cameraRotation;*/
		
		bool m_is_controlled_by_keyboard_mouse = false;
		Vector2 m_mouse_last_position = Vector2::Zero;
		bool m_fps_control_cursor_hidden = false;
		Vector3 m_movement_speed = Vector3::Zero;
		float m_movement_scroll_accumulator = 0.0f;
		Vector2 m_mouse_smoothed = Vector2::Zero;
		Vector2 m_first_person_rotation = Vector2::Zero;
		float m_mouse_sensitivity = 0.05f;
		float m_mouse_smoothing = 0.05f;
	
	};
}
