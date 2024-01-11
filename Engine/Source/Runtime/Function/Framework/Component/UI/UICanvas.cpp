
#include "UICanvas.h"

#include "RectTransform.h"

namespace LitchiRuntime
{
	UICanvas::UICanvas():Component(),m_resolution(1920,1080)
	{
	}
	UICanvas::~UICanvas()
	{
	}
	void UICanvas::Awake()
	{
		// find or add camera
		if(!m_canvasCamera)
		{
			m_canvasCamera = new RenderCamera();
		}

		// init RectTramsform
		InitCanvasTransform();

		// init camera
		InitCanvasCamera();
	}

	void UICanvas::Update()
	{
	}

	void UICanvas::InitCanvasCamera()
	{
		const float CameraNearPlane = 5.0f;
		const float CameraFarPlane = 1000.0f;

		m_canvasCamera->SetNearPlane(CameraNearPlane);
		m_canvasCamera->SetFarPlane(CameraFarPlane);
		m_canvasCamera->SetProjection(Projection_Orthographic);
		m_canvasCamera->SetViewport(m_resolution.x, m_resolution.y);

		Transform* transform = GetGameObject()->GetComponent<Transform>();
		auto canvasRotation= transform->GetRotation();
		auto canvasPos= transform->GetPosition();
		canvasPos.z -= CameraNearPlane;
		m_canvasCamera->SetRotation(canvasRotation);
		m_canvasCamera->SetPosition(canvasPos);

	}
	void UICanvas::InitCanvasTransform()
	{
		Transform* transform = GetGameObject()->GetComponent<Transform>();
		if(transform)
		{
			Vector3 canvasPos = Vector3(m_resolution.x / 2.0f, m_resolution.y / 2.0f, 0.0f);
			transform->SetPosition(canvasPos);
		}
	}
}
