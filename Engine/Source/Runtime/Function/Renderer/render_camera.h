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

        /// �������λ�� ����
        /// \param cameraFowrad �����ǰ����
        /// \param cameraUp ������Ϸ���
        void SetAndUpdateView(const glm::vec3& cameraPos,const glm::vec3& centerPos, const glm::vec3& cameraUp);

        /// ���������Ұ
        /// \param fovDegrees   �����Ұ ���ӽǶ�
        /// \param aspectRatio  ��߱�
        /// \param nearClip ���ü���
        /// \param farClip  Զ�ü���
        void UpdateProjection();

        float GetFov () { return fov_; }
        void SetFov(float fovDegrees)
        {
            fov_ = std::clamp(fovDegrees, MIN_FOV, MAX_FOV);
        }

        float GetAspectRatio() { return aspectRatio_; }
        void SetAspectRatio(float aspectRatio) { aspectRatio_ = aspectRatio; }

        /// ����������ɫ
        /// \param r
        /// \param g
        /// \param b
        /// \param a
        void set_clear_color(float r, float g, float b, float a) { clear_color_ = glm::vec4(r, g, b, a); }

        /// ����ˢ֡������������
        /// \param clear_flag
        void set_clear_flag(unsigned int clear_flag) { clear_flag_ = clear_flag; }

        /// ˢ֡����
        void Clear();

        /// ���target_render_texture_�Ƿ����ã�����ʹ��FBO����Ⱦ��RenderTexture��
        void CheckRenderToTexture();

        /// ����Ƿ�Ҫȡ��ʹ��RenderTexture.
        void CheckCancelRenderToTexture();

        /// ������ȾĿ��RenderTexture
        /// \param render_texture
        void set_target_render_texture(RenderTexture* render_texture);

        /// �����ȾĿ��RenderTexture
        void clear_target_render_texture();

        glm::mat4& view_mat4() { return view_mat4_; }
        glm::mat4& projection_mat4() { return projection_mat4_; }

	private:

        glm::mat4 view_mat4_;//ָ���������ͳ���
        glm::mat4 projection_mat4_;//ָ�������Χ

        float fov_;

        float aspectRatio_;
        float nearClip_;
        float farClip_;

        glm::vec4 clear_color_;//������ɫ
        unsigned int clear_flag_;//ˢ�����ݱ�־

        unsigned char depth_;//�������

        unsigned char culling_mask_;//������Ⱦ��ЩLayer������

        RenderTexture* target_render_texture_;//��ȾĿ��RenderTexture

	};
}
