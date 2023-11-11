
#pragma once

#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Renderer/RenderCamera.h"
#include "Runtime/Function/UI/Widgets/AWidget.h"

namespace LitchiRuntime
{
	/**
	* Simple widget that display TransformGizmo
	*/
	class TransformGizmo : public AWidget
	{
	public:
		TransformGizmo(RenderCamera* m_camera);
		void SetSelectGameObject(GameObject* go);
		void SetCamera(RenderCamera* m_camera);
	protected:
		virtual void _Draw_Impl() override;
	private:
		RenderCamera* m_camera;
		GameObject* m_selectObject = nullptr;
	};
}
