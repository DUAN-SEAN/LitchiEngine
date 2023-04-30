#pragma once


#include <algorithm>
#include <glm.hpp>

namespace LitchiRuntime
{
    class RenderTexture;
	class RenderCamera
	{
	public:

        RenderCamera();
        ~RenderCamera();

        static constexpr float MIN_FOV{ 10.0f };
        static constexpr float MAX_FOV{ 89.0f };

        /// 设置相机位置 朝向
        /// \param cameraFowrad 相机朝前方向
        /// \param cameraUp 相机朝上方向
        void SetAndUpdateView(const glm::vec3& cameraPos,const glm::vec3& centerPos, const glm::vec3& cameraUp);

        /// 设置相机视野
        /// \param fovDegrees   相机视野 可视角度
        /// \param aspectRatio  宽高比
        /// \param nearClip 近裁剪面
        /// \param farClip  远裁剪面
        void UpdateProjection();

        float GetFov () { return fov_; }
        void SetFov(float fovDegrees)
        {
            fov_ = std::clamp(fovDegrees, MIN_FOV, MAX_FOV);
        }

        float GetAspectRatio() { return aspectRatio_; }
        void SetAspectRatio(float aspectRatio) { aspectRatio_ = aspectRatio; }

        /// 设置清屏颜色
        /// \param r
        /// \param g
        /// \param b
        /// \param a
        void set_clear_color(float r, float g, float b, float a) { clear_color_ = glm::vec4(r, g, b, a); }

        /// 设置刷帧清屏内容种类
        /// \param clear_flag
        void set_clear_flag(unsigned int clear_flag) { clear_flag_ = clear_flag; }

        /// 刷帧清屏
        void Clear();

        /// 检查target_render_texture_是否设置，是则使用FBO，渲染到RenderTexture。
        void CheckRenderToTexture();

        /// 检查是否要取消使用RenderTexture.
        void CheckCancelRenderToTexture();

        /// 设置渲染目标RenderTexture
        /// \param render_texture
        void set_target_render_texture(RenderTexture* render_texture);

        /// 清空渲染目标RenderTexture
        void clear_target_render_texture();

        glm::mat4& view_mat4() { return view_mat4_; }
        glm::mat4& projection_mat4() { return projection_mat4_; }

	private:

        glm::mat4 view_mat4_;//指定相机坐标和朝向
        glm::mat4 projection_mat4_;//指定相机范围

        float fov_;

        float aspectRatio_;
        float nearClip_;
        float farClip_;

        glm::vec4 clear_color_;//清屏颜色
        unsigned int clear_flag_;//刷新数据标志

        unsigned char depth_;//排序深度

        unsigned char culling_mask_;//控制渲染哪些Layer的物体

        RenderTexture* target_render_texture_;//渲染目标RenderTexture

	};
}
