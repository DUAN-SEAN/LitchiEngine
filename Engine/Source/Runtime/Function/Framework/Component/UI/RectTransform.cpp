#include "RectTransform.h"

#include "UICanvas.h"

namespace LitchiRuntime
{
	RectTransform::RectTransform() :Transform(),m_pos(Vector3::Zero), m_size(10.0f, 10.0f)
	{
	}
	RectTransform::~RectTransform()
	{
	}
	void RectTransform::OnAwake()
	{
	}

	void RectTransform::OnUpdate()
	{
		UpdateRectTransform();
	}

	void RectTransform::OnEditorUpdate()
	{
		UpdateRectTransform();
	}

	void RectTransform::PostResourceLoaded()
	{
		PostResourceModify();
	}

	void RectTransform::PostResourceModify()
	{
		UpdateRectTransform();
	}

	void RectTransform::UpdateRectTransform()
	{
		UpdateCanvas();

		if (m_canvas)
		{
			// get canvas transform
			const auto canvasTrans = m_canvas->GetGameObject()->GetComponent<Transform>();
			const auto resolution = m_canvas->GetResolution();

			// calc rect pos relative by canvas
			const auto rectCanvasPos = Vector3(m_pos.x, resolution.y-m_pos.y, m_pos.z);

			// calc world pos 
			const auto canvasCenterPos = Vector3(resolution.x / 2, resolution.y / 2, 0.0f);
			const auto canvasOffsetPos = canvasTrans->GetPosition() - canvasCenterPos;
			const auto rectTransWorldPos = rectCanvasPos + canvasOffsetPos;

			// todo: rotation
			SetScale(Vector3(m_size.x, m_size.y,1.0f));
			SetPosition(rectTransWorldPos);
		}
	}

	void RectTransform::UpdateCanvas()
	{
		if (!m_canvas)
		{
			if (GetGameObject()->HasParent())
			{
				m_canvas = GetGameObject()->GetParent()->GetComponent<UICanvas>();
			}
		}
	}


}

