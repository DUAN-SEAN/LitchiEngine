
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
	}

	void UICanvas::Update()
	{
		Transform* transform = GetGameObject()->GetComponent<Transform>();
		transform->SetPosition(Vector3::Zero);
	}

	void UICanvas::PostResourceLoaded()
	{
		// init RectTramsform
		InitCanvasTransform();

		// init camera
		InitCanvasCamera();
	}

	void UICanvas::InitCanvasCamera()
	{
		// do nothing
	}
	void UICanvas::InitCanvasTransform()
	{
		Transform* transform = GetGameObject()->GetComponent<Transform>();
		transform->SetPosition(Vector3::Zero);
	}
}
