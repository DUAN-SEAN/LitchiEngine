
#pragma once

#include "Runtime/Core/Math/Frustum.h"
#include "Runtime/Core/Math/Vector3.h"
#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Renderer/RenderCamera.h"
#include "Runtime/Function/Renderer/RHI/RHI_Texture.h"

namespace LitchiRuntime
{
	class MeshFilter;
	class Camera;

	enum class LightType
	{
		Directional,
		Point,
		Spot
	};

	enum class LightIntensity
	{
		sky_sunlight_noon,            // Direct sunlight at noon, the brightest light
		sky_sunlight_morning_evening, // Direct sunlight at morning or evening, less intense than noon light
		sky_overcast_day,             // Light on an overcast day, considerably less intense than direct sunlight
		sky_twilight,                 // Light just after sunset, a soft and less intense light
		bulb_stadium,                 // Intense light used in stadiums for sports events, comparable to sunlight
		bulb_500_watt,                // A very bright domestic bulb or small industrial light
		bulb_150_watt,                // A bright domestic bulb, equivalent to an old-school incandescent bulb
		bulb_100_watt,                // A typical bright domestic bulb
		bulb_60_watt,                 // A medium intensity domestic bulb
		bulb_25_watt,                 // A low intensity domestic bulb, used for mood lighting or as a night light
		bulb_flashlight,              // Light emitted by an average flashlight, portable and less intense
		black_hole,                   // No light emitted
		custom                        // Custom intensity
	};

	struct ShadowSlice
	{
		Vector3 min = Vector3::Zero;
		Vector3 max = Vector3::Zero;
		Vector3 center = Vector3::Zero;
		Frustum frustum;
	};

	struct ShadowMap
	{
		std::shared_ptr<RHI_Texture> texture_color;
		std::shared_ptr<RHI_Texture> texture_depth;
		std::vector<ShadowSlice> slices;
	};
	/**
	* Base class for any light
	*/
	class Light : public Component
	{
	public:

        Light();
        ~Light();

        void Update() override;

		LightType GetLightType() { return m_light_type; }
        void SetLightType(LightType type);

        // Color
        void SetTemperature(const float temperature_kelvin);
        float GetTemperature() const { return m_temperature_kelvin; }
        void SetColor(Color rgb);
		Color GetColor() { return m_color_rgb; }

        // Intensity
        void SetIntensityLumens(float lumens);
        void SetIntensity(const LightIntensity lumens);
        float GetIntensityLumens() { return m_intensity_lumens; }
        LightIntensity GetIntensity() const { return m_intensity; }
        float GetIntensityWatt(RenderCamera* camera) const;

        bool GetShadowsEnabled() const { return m_shadows_enabled; }
        void SetShadowsEnabled(bool cast_shadows);

        bool GetShadowsTransparentEnabled() const { return m_shadows_transparent_enabled; }
        void SetShadowsTransparentEnabled(bool cast_transparent_shadows);

        bool GetVolumetricEnabled() const { return m_volumetric_enabled; }
        void SetVolumetricEnabled(bool is_volumetric) { m_volumetric_enabled = is_volumetric; }

        void SetRange(float range);
        auto GetRange() const { return m_range; }

        void SetAngle(float angle_rad);
        auto GetAngle() const { return m_angle_rad; }

        void SetBias(float value) { m_bias = value; }
        float GetBias() const { return m_bias; }

        void SetNormalBias(float value) { m_normal_bias = value; }
        auto GetNormalBias() const { return m_normal_bias; }

        const Matrix& GetViewMatrix(uint32_t index = 0) const;
        const Matrix& GetProjectionMatrix(uint32_t index = 0) const;

        RHI_Texture* GetDepthTexture() const { return m_shadow_map.texture_depth.get(); }
        RHI_Texture* GetColorTexture() const { return m_shadow_map.texture_color.get(); }
        uint32_t GetShadowArraySize() const;
        void CreateShadowMap();

        bool IsInViewFrustum(MeshFilter* renderable, uint32_t index) const;

        RTTR_ENABLE(Component)

    private:
        void ComputeViewMatrix();
        void ComputeProjectionMatrix(uint32_t index = 0);
        void ComputeCascadeSplits();

        // Intensity
        LightIntensity m_intensity = LightIntensity::bulb_500_watt;
        float m_intensity_lumens = 2600.0f;

        // Shadows
        bool m_shadows_enabled = true;
        bool m_shadows_transparent_enabled = true;
        uint32_t m_cascade_count = 4;
        ShadowMap m_shadow_map;

        // Bias
        float m_bias = 0.0f;
        float m_normal_bias = 5.0f;

        // Misc
        LightType m_light_type = LightType::Directional;
        Color m_color_rgb = Color::standard_black;;
        float m_temperature_kelvin = 0.0f;
        bool m_volumetric_enabled = true;
        float m_range = 10.0f;
        float m_angle_rad = 0.5f; // about 30 degrees
        bool m_initialized = false;
        std::array<Matrix, 6> m_matrix_view;
        std::array<Matrix, 6> m_matrix_projection;

        // Dirty checks
        bool m_is_dirty = true;
        Matrix m_previous_camera_view = Matrix::Identity;
	protected:
	};
}
