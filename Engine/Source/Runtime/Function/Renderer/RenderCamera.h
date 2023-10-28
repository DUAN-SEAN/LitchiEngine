#pragma once


#include <algorithm>

#include "Runtime/Core/DataStruct/Frustum.h"
#include "Runtime/Core/Math/Frustum.h"

namespace LitchiRuntime
{
	/**
    * Projection modes, mostly used for cameras
    */
    enum class ProjectionMode
    {
        ORTHOGRAPHIC,
        PERSPECTIVE
    };
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
        void SetAndUpdateView(const Vector3& cameraPos,const Vector3& centerPos, const Vector3& cameraUp);

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
        void set_clear_color(float r, float g, float b, float a) { clear_color_ = Vector4(r, g, b, a); }

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

        Matrix& view_mat4() { return view_mat4_; }
        Matrix& projection_mat4() { return projection_mat4_; }

	public:
		/**
		* Cache the projection, view and frustum matrices
		* @param p_windowWidth
		* @param p_windowHeight
		* @param p_position
		* @param p_rotation
		*/
		void CacheMatrices(uint16_t p_windowWidth, uint16_t p_windowHeight, const Vector3& p_position, const Quaternion& p_rotation);

		/**
		* Calculate and cache the result projection matrix
		* @param p_windowWidth
		* @param p_windowHeight
		*/
		void CacheProjectionMatrix(uint16_t p_windowWidth, uint16_t p_windowHeight);

		/**
		* Calculate and cache the result view matrix
		* @param p_position
		* @param p_rotation
		*/
		void CacheViewMatrix(const Vector3& p_position, const Quaternion& p_rotation);

		/**
		* Calculate and cache the result frustum.
		* This method should be called after projection and view matrices are cached.
		* @param p_view
		* @param p_projection
		*/
		void CacheFrustum(const Matrix& p_view, const Matrix& p_projection);

		/**
		* Returns the fov of the camera
		*/
		float GetFov() const;

		/**
		* Returns the size of the camera
		*/
		float GetSize() const;

		/**
		* Returns the near of the camera
		*/
		float GetNear() const;

		/**
		* Returns the far of the camera
		*/
		float GetFar() const;

		/**
		* Returns the clear color of the camera
		*/
		const Vector3& GetClearColor() const;

		/**
		* Returns the cached projection matrix
		*/
		const Matrix& GetProjectionMatrix() const;

		/**
		* Returns the cached view matrix
		*/
		const Matrix& GetViewMatrix() const;

		/**
		* Retursn the cached frustum
		*/
		const LitchiRuntime::Frustum& GetFrustum() const;

		/**
		* Returns true if the frustum culling for geometry is enabled
		*/
		bool HasFrustumGeometryCulling() const;

		/**
		* Returns true if the frustum culling for lights is enabled
		*/
		bool HasFrustumLightCulling() const;

		/**
		* Returns the current projection mode
		*/
		ProjectionMode GetProjectionMode() const;

		///**
		//* Sets the fov of the camera to the given value
		//* @param p_value
		//*/
		//void SetFov(float p_value);

		/**
		* Sets the size of the camera to the given value
		* @param p_value
		*/
		void SetSize(float p_value);

		/**
		* Sets the near of the camera to the given value
		* @param p_value
		*/
		void SetNear(float p_value);

		/**
		* Sets the far of the camera to the given value
		* @param p_value
		*/
		void SetFar(float p_value);

		/**
		* Sets the clear color of the camera to the given value
		* @param p_value
		*/
		void SetClearColor(const Vector3& p_clearColor);

		/**
		* Defines if the camera should apply frustum culling to geometry while rendering
		* @param p_enable
		*/
		void SetFrustumGeometryCulling(bool p_enable);

		/**
		* Defines if the camera should apply frustum culling to lights while rendering
		* @param p_enable
		*/
		void SetFrustumLightCulling(bool p_enable);

		/**
		* Defines the projection mode the camera should adopt
		* @param p_projectionMode
		*/
		void SetProjectionMode(ProjectionMode p_projectionMode);


	private:

		Matrix CalculateProjectionMatrix(uint16_t p_windowWidth, uint16_t p_windowHeight) const;
		Matrix CalculateViewMatrix(const Vector3 &p_position, const Quaternion& p_rotation) const;
	private:

		Frustum m_frustum;

        Matrix view_mat4_;//ָ���������ͳ���
        Matrix projection_mat4_;//ָ�������Χ

        ProjectionMode m_projectionMode;

        float fov_;

        float aspectRatio_;
        float nearClip_;
        float farClip_;
		float m_size;

        Vector4 clear_color_;//������ɫ
        unsigned int clear_flag_;//ˢ�����ݱ�־

        unsigned char depth_;//�������

        unsigned char culling_mask_;//������Ⱦ��ЩLayer������

        RenderTexture* target_render_texture_;//��ȾĿ��RenderTexture


        bool m_frustumGeometryCulling;
        bool m_frustumLightCulling;

	};
}
