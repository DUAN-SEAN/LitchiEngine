
#pragma once

//= INCLUDES ===============
#include "Runtime/Core/Math/Vector2.h"
#include "Runtime/Core/Math/Vector3.h"
#include "Runtime/Core/Math/Matrix.h"
#include "Color.h"
//==========================

namespace LitchiRuntime
{
	// low frequency - updates once per frame
	struct Cb_Frame
	{
		Matrix view;
		Matrix projection;
		Matrix view_projection;
		Matrix view_projection_inv;
		Matrix view_projection_ortho;
		Matrix view_projection_unjittered;
		Matrix view_projection_previous;

		Vector2 resolution_render;
		Vector2 resolution_output;

		Vector2 taa_jitter_current;
		Vector2 taa_jitter_previous;

		float delta_time;
		uint32_t frame;
		float gamma;
		uint32_t options;

		Vector3 camera_position;
		float camera_near;

		Vector3 camera_direction;
		float camera_far;

		void set_bit(const bool set, const uint32_t bit)
		{
			options = set ? (options |= bit) : (options & ~bit);
		}

		bool operator==(const Cb_Frame& rhs) const
		{
			return
				view == rhs.view &&
				projection == rhs.projection &&
				view_projection == rhs.view_projection &&
				view_projection_inv == rhs.view_projection_inv &&
				view_projection_ortho == rhs.view_projection_ortho &&
				view_projection_unjittered == rhs.view_projection_unjittered &&
				view_projection_previous == rhs.view_projection_previous &&
				delta_time == rhs.delta_time &&
				frame == rhs.frame &&
				camera_near == rhs.camera_near &&
				camera_far == rhs.camera_far &&
				camera_position == rhs.camera_position &&
				camera_direction == rhs.camera_direction &&
				gamma == rhs.gamma &&
				resolution_output == rhs.resolution_output &&
				resolution_render == rhs.resolution_render &&
				taa_jitter_current == rhs.taa_jitter_current &&
				taa_jitter_previous == rhs.taa_jitter_previous &&
				options == rhs.options;
		}

		bool operator!=(const Cb_Frame& rhs) const { return !(*this == rhs); }
	};

	// low frequency - updates once per rendererPath
	struct Cb_RendererPath
	{
		Matrix view;
		Matrix projection;
		Matrix view_projection;
		Matrix view_projection_inv;
		Matrix view_projection_ortho;
		Matrix view_projection_unjittered;
		Matrix view_projection_previous;
		
		Vector3 camera_position;
		float camera_near;

		Vector3 camera_direction;
		float camera_far;
		
		bool operator==(const Cb_Frame& rhs) const
		{
			return
				view == rhs.view &&
				projection == rhs.projection &&
				view_projection == rhs.view_projection &&
				view_projection_inv == rhs.view_projection_inv &&
				view_projection_ortho == rhs.view_projection_ortho &&
				view_projection_unjittered == rhs.view_projection_unjittered &&
				view_projection_previous == rhs.view_projection_previous &&
				camera_near == rhs.camera_near &&
				camera_far == rhs.camera_far &&
				camera_position == rhs.camera_position &&
				camera_direction == rhs.camera_direction;
		}

		bool operator!=(const Cb_Frame& rhs) const { return !(*this == rhs); }
	};

	// medium frequency - updates per light
	struct Cb_Light
	{
		Matrix view_projection[6];
		Vector4 intensity_range_angle_bias;
		Color color;
		Vector4 position;
		Vector4 direction;
		float normal_bias;
		uint32_t options;
		Vector2 padding;

		bool operator==(const Cb_Light& rhs)
		{
			return
				view_projection[0] == rhs.view_projection[0] &&
				view_projection[1] == rhs.view_projection[1] &&
				view_projection[2] == rhs.view_projection[2] &&
				view_projection[3] == rhs.view_projection[3] &&
				view_projection[4] == rhs.view_projection[4] &&
				view_projection[5] == rhs.view_projection[5] &&
				intensity_range_angle_bias == rhs.intensity_range_angle_bias &&
				normal_bias == rhs.normal_bias &&
				color == rhs.color &&
				position == rhs.position &&
				direction == rhs.direction &&
				options == rhs.options;
		}
	};

	// medium to high frequency - updates per light
	struct Cb_Material
	{
		Vector4 color = Vector4::Zero;

		Vector2 tiling_uv = Vector2::Zero;
		Vector2 offset_uv = Vector2::Zero;

		float roughness_mul = 0.0f;
		float metallic_mul = 0.0f;
		float normal_mul = 0.0f;
		float height_mul = 0.0f;

		uint32_t properties = 0;
		float clearcoat = 0.0f;
		float clearcoat_roughness = 0.0f;
		float anisotropic = 0.0f;

		float anisitropic_rotation = 0.0f;
		float sheen = 0.0f;
		float sheen_tint = 0.0f;
		float padding = 0.0f;

		bool operator==(const Cb_Material& rhs) const
		{
			return
				color == rhs.color &&
				tiling_uv == rhs.tiling_uv &&
				offset_uv == rhs.offset_uv &&
				roughness_mul == rhs.roughness_mul &&
				metallic_mul == rhs.metallic_mul &&
				normal_mul == rhs.normal_mul &&
				height_mul == rhs.height_mul &&
				properties == rhs.properties &&
				clearcoat == rhs.clearcoat &&
				clearcoat_roughness == rhs.clearcoat_roughness &&
				anisotropic == rhs.anisotropic &&
				anisitropic_rotation == rhs.anisitropic_rotation &&
				sheen == rhs.sheen &&
				sheen_tint == rhs.sheen_tint;
		}
	};

	const static int MaxLightCount = 15;

	struct Cb_Light_Arr
	{
		int lightCount;
		int padding[3];
		Cb_Light lightArr[MaxLightCount];
		bool operator==(const Cb_Light_Arr& rhs)
		{
			if (lightCount != rhs.lightCount)
			{
				return false;
			}

			for (int index = 0; index < lightCount; index++)
			{
				auto& light = lightArr[index];
				auto& rhsLight = rhs.lightArr[index];

				if(light == rhsLight)
				{
					continue;
				}

				return  false;
				
			}

			return true;
		}
	};

	const static int MaxBoneCount = 250;

	struct Cb_Bone_Arr
	{
		/*int boneCount;
		int padding[3];*/
		Matrix boneArr[MaxBoneCount];
	};

	// 128 byte push constant buffer - updates per pass/draw
	struct Pcb_Pass
	{
		Matrix transform = Matrix::Identity;
		Matrix m_value = Matrix::Identity;

		void set_transform_previous(const Matrix& transform_previous)
		{
			m_value = transform_previous;
		}

		void set_resolution_in(const Vector2& resolution)
		{
			m_value.m03 = resolution.x;
			m_value.m22 = resolution.y;
		};

		void set_resolution_out(const RHI_Texture* texture)
		{
			m_value.m23 = static_cast<float>(texture->GetWidth());
			m_value.m30 = static_cast<float>(texture->GetHeight());
		};

		void set_resolution_out(const Vector2& resolution)
		{
			m_value.m23 = resolution.x;
			m_value.m30 = resolution.y;
		};

		void set_f3_value(const Vector3& value)
		{
			m_value.m00 = value.x;
			m_value.m01 = value.y;
			m_value.m02 = value.z;
		};

		void set_f3_value(const float x, const float y, const float z)
		{
			m_value.m00 = x;
			m_value.m01 = y;
			m_value.m02 = z;
		};

		void set_f3_value2(const Vector3& value)
		{
			m_value.m20 = value.x;
			m_value.m21 = value.y;
			m_value.m31 = value.z;
		};

		void set_f3_value2(const float x, const float y, const float z)
		{
			m_value.m20 = x;
			m_value.m21 = y;
			m_value.m31 = z;
		};

		void set_f4_value(const Vector4& color)
		{
			m_value.m10 = color.x;
			m_value.m11 = color.y;
			m_value.m12 = color.z;
			m_value.m13 = color.w;
		};

		void set_f4_value(const Color& color)
		{
			m_value.m10 = color.r;
			m_value.m11 = color.g;
			m_value.m12 = color.b;
			m_value.m13 = color.a;
		};

		void set_f4_value(const float x, const float y, const float z, const float w)
		{
			m_value.m10 = x;
			m_value.m11 = y;
			m_value.m12 = z;
			m_value.m13 = w;
		};

		void set_is_transparent(const bool is_transparent)
		{
			m_value.m33 = is_transparent ? 1.0f : 0.0f;
		}

		bool operator==(const Pcb_Pass& rhs) const
		{
			return transform == rhs.transform && m_value == rhs.m_value;
		}

		bool operator!=(const Pcb_Pass& rhs) const { return !(*this == rhs); }
	};
}
