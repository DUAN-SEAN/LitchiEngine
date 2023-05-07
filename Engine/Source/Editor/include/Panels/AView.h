
#pragma once

#include <string>

#include "core/type.hpp"
#include "Editor/include/Core/CameraController.h"
#include "gtc/quaternion.hpp"
#include "Runtime/Function/Renderer/Framebuffer.h"
#include "Runtime/Function/UI/Panels/PanelWindow.h"
#include "Runtime/Function/UI/Settings/PanelWindowSettings.h"
#include "Runtime/Function/UI/Widgets/Visual/Image.h"

namespace LitchiEditor { class EditorRenderer; }

using namespace LitchiRuntime;
namespace LitchiEditor
{
	/**
	* Base class for any view
	*/
	class AView : public PanelWindow
	{
	public:
		/**
		* Constructor
		* @param p_title
		* @param p_opened
		* @param p_windowSettings
		*/
		AView
		(
			const std::string& p_title,
			bool p_opened,
			const PanelWindowSettings& p_windowSettings
		);

		/**
		* Update the view
		* @param p_deltaTime
		*/
		virtual void Update(float p_deltaTime);

		/**
		* Custom implementation of the draw method
		*/
		void _Draw_Impl() override;

		/**
		* Custom implementation of the render method to define in dervied classes
		*/
		virtual void _Render_Impl() = 0;

		/**
		* Render the view
		*/
		void Render();

		/**
		* Defines the camera position
		* @param p_position
		*/
		void SetCameraPosition(const glm::vec3& p_position);

		/**
		* Defines the camera rotation
		* @param p_rotation
		*/
		void SetCameraRotation(const glm::quat& p_rotation);

		/**
		* Returns the camera position
		*/
		const glm::vec3& GetCameraPosition() const;

		/**
		* Returns the camera rotation
		*/
		const glm::quat& GetCameraRotation() const;

		/**
		* Returns the camera used by this view
		*/
		RenderCamera& GetCamera();

		/**
		* Returns the size of the panel ignoring its titlebar height
		*/
		std::pair<uint16_t, uint16_t> GetSafeSize() const;

		/**
		* Returns the grid color of the view
		*/
		const glm::vec3& GetGridColor() const;

		/**
		* Defines the grid color of the view
		* @param p_color
		*/
		void SetGridColor(const glm::vec3& p_color);

		/**
		* Fill the UBO using the view settings
		*/
		void FillEngineUBO();

	protected:
		/**
		* Update camera matrices
		*/
		void PrepareCamera();

	protected:
		EditorRenderer& m_editorRenderer;
		RenderCamera* m_camera;
		glm::vec3 m_cameraPosition;
		glm::quat m_cameraRotation;
		Image* m_image;

        glm::vec3 m_gridColor = glm::vec3 { 0.176f, 0.176f, 0.176f };

		Framebuffer m_fbo;
	};
}