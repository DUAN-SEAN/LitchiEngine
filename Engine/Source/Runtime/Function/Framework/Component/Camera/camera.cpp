
#include "camera.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include "Runtime/Function/Renderer/RenderCamera.h"
#include "Runtime/Function/Renderer/RenderSystem.h"

namespace LitchiRuntime
{

	Camera::Camera() :m_depth(0), m_culling_mask(0x01) {
		
	}
	
	Camera::~Camera() {
	}

	/*void Camera::SetAndUpdateView(const glm::vec3& centerPos, const glm::vec3& cameraUp) {
		auto transform = game_object()->GetComponent<Transform>();
		RenderSystem::Instance()->GetRenderContext()->main_render_camera_->SetAndUpdateView(transform->position(), centerPos, cameraUp);
	}

	void Camera::SetAndUpdateProjection(float fovDegrees, float aspectRatio, float nearClip, float farClip) {

		RenderSystem::Instance()->GetRenderContext()->main_render_camera_->SetAndUpdateProjection(glm::radians(fovDegrees), aspectRatio, nearClip, farClip);
	}
	
	void Camera::set_clear_color(float r, float g, float b, float a)
	{
		RenderSystem::Instance()->GetRenderContext()->main_render_camera_->set_clear_color(r, g, b, a);
	}

	void Camera::set_clear_flag(unsigned int clear_flag)
	{
		RenderSystem::Instance()->GetRenderContext()->main_render_camera_->set_clear_flag(clear_flag);
	}*/

	void Camera::SetDepth(unsigned char depth) {
		if (m_depth == depth) {
			return;
		}
		m_depth = depth;
		// Sort();
	}

	void Camera::Update()
	{
		auto transform = GetGameObject()->GetComponent<Transform>();

		// 更新viewMatrix
		RenderSystem::Instance()->GetRenderContext()->main_render_camera_->SetAndUpdateView(transform->GetWorldPosition(),glm::vec3(0,0,0),glm::vec3(0,1,0));

		RenderSystem::Instance()->GetRenderContext()->main_render_camera_->SetFov(60.0);

		// todo depth mask
		
	}

	//void Camera::Sort() {
	//	std::sort(all_camera_.begin(), all_camera_.end(), [](Camera* a, Camera* b) {
	//		return a->depth() < b->depth();
	//		});
	//}

	//void Camera::Foreach(std::function<void()> func) {
	//	for (auto iter = all_camera_.begin(); iter != all_camera_.end(); iter++) {
	//		current_camera_ = *iter;
	//		// current_camera_->CheckRenderToTexture();
	//		current_camera_->Clear();
	//		func();
	//		//current_camera_->CheckCancelRenderToTexture();
	//	}
	//}

}











