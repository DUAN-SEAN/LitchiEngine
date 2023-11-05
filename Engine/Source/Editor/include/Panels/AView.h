
#pragma once

#include <string>

#include "Runtime/Core/Math/Quaternion.h"
#include "Runtime/Function/Renderer/Resources/Shader.h"
#include "Runtime/Function/Renderer/RHI/RHI_Texture.h"
#include "Runtime/Function/UI/Panels/PanelWindow.h"
#include "Runtime/Function/UI/Settings/PanelWindowSettings.h"
#include "Runtime/Function/UI/Widgets/Visual/Image.h"

// namespace LitchiEditor { class EditorRenderer; }
namespace LitchiRuntime { class RenderCamera; }
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
			const PanelWindowSettings& p_windowSettings,
			RHI_Texture* renderTargetTexture
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
		void SetCameraPosition(const Vector3& p_position);

		/**
		* Defines the camera rotation
		* @param p_rotation
		*/
		void SetCameraRotation(const Quaternion& p_rotation);

		/**
		* Returns the camera position
		*/
		const Vector3& GetCameraPosition() const;

		/**
		* Returns the camera rotation
		*/
		const Quaternion& GetCameraRotation() const;

		/**
		* Returns the camera used by this view
		*/
		RenderCamera* GetCamera();

		/**
		* Returns the size of the panel ignoring its titlebar height
		*/
		std::pair<uint16_t, uint16_t> GetSafeSize() const;

		/**
		* Returns the grid color of the view
		*/
		const Vector3& GetGridColor() const;

		/**
		* Defines the grid color of the view
		* @param p_color
		*/
		void SetGridColor(const Vector3& p_color);
		
	protected:
		/**
		* Update camera matrices
		*/
		void PrepareCamera();

	protected:
		//EditorRenderer& editorRenderer;
		RenderCamera* m_camera;
		Vector3 m_cameraPosition;
		Quaternion m_cameraRotation;
		Image* m_image;
		RHI_Texture* m_renderTargetTexture;

        Vector3 m_gridColor = Vector3 { 0.176f, 0.176f, 0.176f };
		
	};
}