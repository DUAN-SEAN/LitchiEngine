
#include "render_camera.h"

#include <glad/glad.h>

#include "gtx/quaternion.hpp"
#include "gtc/quaternion.hpp"
#include "gtc/matrix_transform.hpp"

#include "Runtime/Core/Log/debug.h"
#include "Runtime/Core/Screen/screen.h"
#include "Runtime/Function/Renderer/gpu_resource_mapper.h"
#include "Runtime/Function/Renderer/render_texture.h"

namespace LitchiRuntime
{
	RenderCamera::RenderCamera() :
		clear_color_(49.f / 255, 77.f / 255, 121.f / 255, 1.f),
		clear_flag_(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT),
		m_size(5.0f),
		fov_(45.0f),
		nearClip_(1.0f),
		farClip_(1000.0f),
		m_frustumGeometryCulling(false),
		m_frustumLightCulling(false),
		m_projectionMode(ProjectionMode::PERSPECTIVE)
	{

	}
	void RenderCamera::SetAndUpdateView(const glm::vec3& cameraPos,const glm::vec3& centerPos, const glm::vec3& cameraUp) {
		view_mat4_ = glm::lookAt(cameraPos, centerPos, cameraUp);
	}

	void RenderCamera::UpdateProjection() {
		projection_mat4_ = glm::perspective(glm::radians(fov_), aspectRatio_, nearClip_, farClip_);
	}

	void RenderCamera::Clear() {
		glClearColor(clear_color_.r, clear_color_.g, clear_color_.b, clear_color_.a);
		glClear(clear_flag_);
	}

	void RenderCamera::CheckRenderToTexture() {

		if (target_render_texture_ == nullptr) {//没有设置目标RenderTexture
			return;
		}
		if (target_render_texture_->in_use()) {
			return;
		}
		if (target_render_texture_->frame_buffer_object_handle() == 0) {//还没有初始化，没有生成FBO。
			return;
		}

		glViewport(0, 0, target_render_texture_->width(), target_render_texture_->height());

		GLuint frame_buffer_object_id = GPUResourceMapper::GetFBO(target_render_texture_->frame_buffer_object_handle());
		glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_object_id); __CHECK_GL_ERROR__
			//检测帧缓冲区完整性，如果完整的话就开始进行绘制
			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER); __CHECK_GL_ERROR__
			if (status != GL_FRAMEBUFFER_COMPLETE) {
				DEBUG_LOG_ERROR("BindFBO FBO Error,Status:{} !", status);
				return;
			}
		target_render_texture_->set_in_use(true);
	}

	void RenderCamera::CheckCancelRenderToTexture() {
		if (target_render_texture_ == nullptr) {//没有设置目标RenderTexture
			return;
		}
		if (target_render_texture_->in_use() == false) {
			return;
		}
		if (target_render_texture_->frame_buffer_object_handle() == 0) {//还没有初始化，没有生成FBO。
			return;
		}
		//更新ViewPort的尺寸
		glViewport(0, 0, Screen::width(), Screen::height());

		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE); __CHECK_GL_ERROR__

			target_render_texture_->set_in_use(false);
	}

	void RenderCamera::set_target_render_texture(RenderTexture* render_texture) {
		if (render_texture == nullptr) {
			clear_target_render_texture();
		}
		target_render_texture_ = render_texture;
	}

	void RenderCamera::clear_target_render_texture() {
		if (target_render_texture_ == nullptr) {//没有设置目标RenderTexture
			return;
		}
		if (target_render_texture_->in_use() == false) {
			return;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE); __CHECK_GL_ERROR__

			target_render_texture_->set_in_use(false);
	}


	void LitchiRuntime::RenderCamera::CacheMatrices(uint16_t p_windowWidth, uint16_t p_windowHeight, const glm::vec3& p_position, const glm::quat& p_rotation)
	{
		SetAspectRatio(p_windowWidth / static_cast<float>(p_windowHeight));
		CacheProjectionMatrix(p_windowWidth, p_windowHeight);
		CacheViewMatrix(p_position, p_rotation);
		CacheFrustum(view_mat4_, projection_mat4_);
	}

	void LitchiRuntime::RenderCamera::CacheProjectionMatrix(uint16_t p_windowWidth, uint16_t p_windowHeight)
	{
		projection_mat4_ = CalculateProjectionMatrix(p_windowWidth, p_windowHeight);
	}

	void LitchiRuntime::RenderCamera::CacheViewMatrix(const glm::vec3& p_position, const glm::quat& p_rotation)
	{
		view_mat4_ = CalculateViewMatrix(p_position, p_rotation);
	}

	void LitchiRuntime::RenderCamera::CacheFrustum(const glm::mat4& p_view, const glm::mat4& p_projection)
	{
		m_frustum.CalculateFrustum(p_projection * p_view);
	}

	float LitchiRuntime::RenderCamera::GetFov() const
	{
		return fov_;
	}

	float LitchiRuntime::RenderCamera::GetSize() const
	{
		return m_size;
	}

	float LitchiRuntime::RenderCamera::GetNear() const
	{
		return nearClip_;
	}

	float LitchiRuntime::RenderCamera::GetFar() const
	{
		return farClip_;
	}

	const glm::vec3& LitchiRuntime::RenderCamera::GetClearColor() const
	{
		return glm::vec3(clear_color_);
	}

	const glm::mat4& LitchiRuntime::RenderCamera::GetProjectionMatrix() const
	{
		return projection_mat4_;
	}

	const glm::mat4& LitchiRuntime::RenderCamera::GetViewMatrix() const
	{
		return view_mat4_;
	}

	const Frustum& LitchiRuntime::RenderCamera::GetFrustum() const
	{
		return m_frustum;
	}

	bool LitchiRuntime::RenderCamera::HasFrustumGeometryCulling() const
	{
		return m_frustumGeometryCulling;
	}

	bool LitchiRuntime::RenderCamera::HasFrustumLightCulling() const
	{
		return m_frustumLightCulling;
	}

	ProjectionMode LitchiRuntime::RenderCamera::GetProjectionMode() const
	{
		return m_projectionMode;
	}
	

	void LitchiRuntime::RenderCamera::SetSize(float p_value)
	{
		m_size = p_value;
	}

	void LitchiRuntime::RenderCamera::SetNear(float p_value)
	{
		nearClip_ = p_value;
	}

	void LitchiRuntime::RenderCamera::SetFar(float p_value)
	{
		farClip_ = p_value;
	}

	void LitchiRuntime::RenderCamera::SetClearColor(const glm::vec3& p_clearColor)
	{
		clear_color_ = glm::vec4(p_clearColor,0);
	}

	void LitchiRuntime::RenderCamera::SetFrustumGeometryCulling(bool p_enable)
	{
		m_frustumGeometryCulling = p_enable;
	}

	void LitchiRuntime::RenderCamera::SetFrustumLightCulling(bool p_enable)
	{
		m_frustumLightCulling = p_enable;
	}

	void LitchiRuntime::RenderCamera::SetProjectionMode(ProjectionMode p_projectionMode)
	{
		m_projectionMode = p_projectionMode;
	}

	glm::mat4 LitchiRuntime::RenderCamera::CalculateProjectionMatrix(uint16_t p_windowWidth, uint16_t p_windowHeight) const
	{
		const auto ratio = aspectRatio_;
		const auto right = m_size * ratio;
		const auto left = -right;
		const auto top = m_size;
		const auto bottom = -top;

		switch (m_projectionMode)
		{
		case ProjectionMode::ORTHOGRAPHIC:
			return glm::ortho(left,right,bottom,top,nearClip_,farClip_);

		case ProjectionMode::PERSPECTIVE:
			return glm::perspective(glm::radians(fov_), aspectRatio_, nearClip_, farClip_);

		default:
			return glm::mat4(1);
		}
	}

	glm::mat4 LitchiRuntime::RenderCamera::CalculateViewMatrix(const glm::vec3& p_position, const glm::quat& p_rotation) const
	{
		glm::vec3 up = glm::normalize(p_rotation) * glm::vec3(0,1.0,0);
			
		glm::vec3 forward = glm::normalize(p_rotation)* glm::vec3(0, 0, -1.0);

	/*	DEBUG_LOG_INFO("---------------------------------");
		DEBUG_LOG_INFO("CalculateViewMatrix eulerRotation.X:{},eulerRotation.Y:{},eulerRotation.Z:{}", eulerRotation.x, eulerRotation.y, eulerRotation.z);
		DEBUG_LOG_INFO("CalculateViewMatrix forward.X:{},forward.Y:{},forward.Z:{}", forward.x, forward.y, forward.z);*/

		return glm::lookAt
		(
			glm::vec3(p_position.x, p_position.y, p_position.z),											// Position
			glm::vec3(p_position.x + forward.x, p_position.y + forward.y, p_position.z + forward.z),			// LookAt (Position + Forward)
			glm::vec3(up.x, up.y, up.z)																		// Up Vector
		);
	}



}
