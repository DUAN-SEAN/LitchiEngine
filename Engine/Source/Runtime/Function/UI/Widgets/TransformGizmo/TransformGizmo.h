
#pragma once

#include "Runtime/Function/Framework/Component/Camera/camera.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/UI/Widgets/AWidget.h"

namespace LitchiRuntime
{
	/**
	* Simple widget that display TransformGizmo
	*/
	class TransformGizmo : public AWidget
	{
	public:
		TransformGizmo(Camera* camera);
		void SetSelectGameObject(GameObject* go);
		void SetCamera(Camera* camera);
	protected:
		virtual void _Draw_Impl() override;
	private:
		Camera* m_camera;
		GameObject* m_selectObject = nullptr;
	};
}
