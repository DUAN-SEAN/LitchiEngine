#include "Runtime/Core/pch.h"
#include "Light.h"

#include "Runtime/Function/Framework/Component/Camera/camera.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshFilter.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Renderer/Rendering/Renderer.h"
#include "Runtime/Function/Renderer/RHI/RHI_Texture2D.h"
#include "Runtime/Function/Renderer/RHI/RHI_Texture2DArray.h"
#include "Runtime/Function/Renderer/RHI/RHI_TextureCube.h"

using namespace LitchiRuntime;

Light::Light() : Component()
{

}

Light::~Light()
{
}

void Light::OnUpdate()
{
}


void Light::SetLightType(LightType type)
{
    if (m_light_type == type)
        return;

    m_light_type = type;
    m_is_dirty = true;
}

void Light::SetTemperature(const float temperature_kelvin)
{
    m_temperature_kelvin = temperature_kelvin;
    m_color_rgb = Color(temperature_kelvin);
}

void Light::SetColor(Color rgb)
{
    m_color_rgb = rgb;

    if (rgb == Color::light_sky_clear)
        m_temperature_kelvin = 15000.0f;
    else if (rgb == Color::light_sky_daylight_overcast)
        m_temperature_kelvin = 6500.0f;
    else if (rgb == Color::light_sky_moonlight)
        m_temperature_kelvin = 4000.0f;
    else if (rgb == Color::light_sky_sunrise)
        m_temperature_kelvin = 2000.0f;
    else if (rgb == Color::light_candle_flame)
        m_temperature_kelvin = 1850.0f;
    else if (rgb == Color::light_direct_sunlight)
        m_temperature_kelvin = 5778.0f;
    else if (rgb == Color::light_digital_display)
        m_temperature_kelvin = 6500.0f;
    else if (rgb == Color::light_fluorescent_tube_light)
        m_temperature_kelvin = 5000.0f;
    else if (rgb == Color::light_kerosene_lamp)
        m_temperature_kelvin = 1850.0f;
    else if (rgb == Color::light_light_bulb)
        m_temperature_kelvin = 2700.0f;
    else if (rgb == Color::light_photo_flash)
        m_temperature_kelvin = 5500.0f;
}


void Light::SetIntensity(const LightIntensity intensity)
{
    m_intensity = intensity;

    if (intensity == LightIntensity::sky_sunlight_noon)
    {
        m_intensity_lumens = 120000.0f;
    }
    else if (intensity == LightIntensity::sky_sunlight_morning_evening)
    {
        m_intensity_lumens = 60000.0f;
    }
    else if (intensity == LightIntensity::sky_overcast_day)
    {
        m_intensity_lumens = 20000.0f;
    }
    else if (intensity == LightIntensity::sky_twilight)
    {
        m_intensity_lumens = 10000.0f;
    }
    else if (intensity == LightIntensity::bulb_stadium)
    {
        m_intensity_lumens = 200000.0f;
    }
    else if (intensity == LightIntensity::bulb_500_watt)
    {
        m_intensity_lumens = 8500.0f;
    }
    else if (intensity == LightIntensity::bulb_150_watt)
    {
        m_intensity_lumens = 2600.0f;
    }
    else if (intensity == LightIntensity::bulb_100_watt)
    {
        m_intensity_lumens = 1600.0f;
    }
    else if (intensity == LightIntensity::bulb_60_watt)
    {
        m_intensity_lumens = 800.0f;
    }
    else if (intensity == LightIntensity::bulb_25_watt)
    {
        m_intensity_lumens = 200.0f;
    }
    else if (intensity == LightIntensity::bulb_flashlight)
    {
        m_intensity_lumens = 100.0f;
    }
    else // black hole
    {
        m_intensity_lumens = 0.0f;
    }
}

void Light::SetIntensityLumens(float lumens)
{
    m_intensity_lumens = lumens;
    m_intensity = LightIntensity::custom;
}

float Light::GetIntensityWatt(RenderCamera* camera) const
{
    LC_ASSERT(camera != nullptr);

    // This magic values are chosen empirically based on how the lights
    // types in the LightIntensity enum should look in the engine
    const float magic_value_a = 50.0f;
    //const float magic_value_b = 0.025f;
    const float magic_value_b = 0.5f;

    // convert lumens to power (in watts) assuming all light is at 555nm
    float power_watts = (m_intensity_lumens / 683.0f) * magic_value_a;

    // for point lights and spot lights, intensity should fall off with the square of the distance,
    // so we don't need to modify the power. For directional lights, intensity should be constant,
    // so we need to multiply the power by the area over which the light is spread
    if (m_light_type == LightType::Directional)
    {
        float area = magic_value_b;
        power_watts *= area;
    }

    // watts can be multiplied by the camera's exposure to get the final intensity
    return power_watts * camera->GetExposure();
}


void Light::SetRange(float range)
{
    m_range = Math::Helper::Clamp(range, 0.0f, std::numeric_limits<float>::max());
    m_is_dirty = true;
}

void Light::SetAngle(float angle)
{
    m_angle_rad = Math::Helper::Clamp(angle, 0.0f, Math::Helper::PI_2);
    m_is_dirty = true;
}

void Light::SetShadowsEnabled(bool cast_shadows)
{
    if (m_shadows_enabled == cast_shadows)
        return;

    m_shadows_enabled = cast_shadows;
    m_is_dirty = true;

    // CreateShadowMap();
}

void Light::SetShadowsTransparentEnabled(bool cast_transparent_shadows)
{
    if (m_shadows_transparent_enabled == cast_transparent_shadows)
        return;

    m_shadows_transparent_enabled = cast_transparent_shadows;
    m_is_dirty = true;
}

void Light::PostResourceLoaded()
{
   
}
