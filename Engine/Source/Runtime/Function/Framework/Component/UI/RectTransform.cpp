#include "RectTransform.h"

#include "UICanvas.h"

namespace LitchiRuntime
{
	void RectTransform::Awake()
	{
		m_canvas = GetGameObject()->GetParent()->GetComponent<UICanvas>();


	}


}

