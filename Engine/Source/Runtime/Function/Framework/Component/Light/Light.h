﻿
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
	};
	/**
	* Base class for any light
	*/
	class Light : public Component
	{
	public:

        Light();
        ~Light();

        void OnUpdate() override;

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

        // bias
        static float GetBias() { return -0.002f; } // small values to avoid disconnected shadows
        static float GetBiasSlopeScaled() { return -2.5f; }

        // range
        void SetRange(float range);
        auto GetRange() { return m_range; }

        // angle
        void SetAngle(float angle_rad);
        auto GetAngle()  { return m_angle_rad; }

        bool GetShadowsEnabled() const { return m_shadows_enabled; }
        void SetShadowsEnabled(bool cast_shadows);

        bool GetShadowsTransparentEnabled() const { return m_shadows_transparent_enabled; }
        void SetShadowsTransparentEnabled(bool cast_transparent_shadows);

        void PostResourceLoaded() override;

        RTTR_ENABLE(Component)

    private:

        bool m_initialized = false;

        // Dirty checks
        bool m_is_dirty = true;

        // Intensity
        LightIntensity m_intensity = LightIntensity::bulb_500_watt;
        float m_intensity_lumens = 2600.0f;

        bool m_shadows_enabled = true;
        bool m_shadows_transparent_enabled = true;

        // Misc
        LightType m_light_type = LightType::Directional;
        Color m_color_rgb = Color::standard_black;;
        float m_temperature_kelvin = 0.0f;
        float m_range = 200.0f;
        float m_angle_rad = 0.5f; // about 30 degrees
	protected:
	};
}
