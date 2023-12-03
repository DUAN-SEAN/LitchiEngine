
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

void Light::Update()
{
	// During engine startup, keep checking until the rhi device gets
        // created so we can create potentially required shadow maps
    if (!m_initialized)
    {
        CreateShadowMap();
        m_initialized = true;
    }

    // Update shadow map(s)
    if (m_shadows_enabled)
    {
        if (m_light_type == LightType::Directional)
        {
            ComputeCascadeSplits();
        }

        ComputeViewMatrix();

        // Compute projection matrix
        if (m_shadow_map.texture_depth)
        {
            for (uint32_t i = 0; i < m_shadow_map.texture_depth->GetArrayLength(); i++)
            {
                ComputeProjectionMatrix(i);
            }
        }
    }

	
}


void Light::SetLightType(LightType type)
{
    if (m_light_type == type)
        return;

    m_light_type = type;
    m_is_dirty = true;

    if (m_shadows_enabled)
    {
        CreateShadowMap();
    }

    // todo
    // World::Resolve();
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
    SP_ASSERT(camera != nullptr);

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


void Light::SetShadowsEnabled(bool cast_shadows)
{
    if (m_shadows_enabled == cast_shadows)
        return;

    m_shadows_enabled = cast_shadows;
    m_is_dirty = true;

    CreateShadowMap();
}

void Light::SetShadowsTransparentEnabled(bool cast_transparent_shadows)
{
    if (m_shadows_transparent_enabled == cast_transparent_shadows)
        return;

    m_shadows_transparent_enabled = cast_transparent_shadows;
    m_is_dirty = true;

    CreateShadowMap();
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

void Light::ComputeViewMatrix()
{
    if (m_light_type == LightType::Directional)
    {
        if (!m_shadow_map.slices.empty())
        {
            for (uint32_t i = 0; i < m_cascade_count; i++)
            {
                ShadowSlice& shadow_map = m_shadow_map.slices[i];
                Vector3 position = shadow_map.center - GetGameObject()->GetComponent<Transform>()->GetForward() * shadow_map.max.z;
                Vector3 target = shadow_map.center;
                Vector3 up = Vector3::Up;
                m_matrix_view[i] = Matrix::CreateLookAtLH(position, target, up);
            }
        }
    }
    else if (m_light_type == LightType::Spot)
    {
        const Vector3 position = GetGameObject()->GetComponent<Transform>()->GetPosition();
        const Vector3 forward = GetGameObject()->GetComponent<Transform>()->GetForward();
        const Vector3 up = GetGameObject()->GetComponent<Transform>()->GetUp();

        // Compute
        m_matrix_view[0] = Matrix::CreateLookAtLH(position, position + forward, up);
    }
    else if (m_light_type == LightType::Point)
    {
        const Vector3 position = GetGameObject()->GetComponent<Transform>()->GetPosition();

        // Compute view for each side of the cube map
        m_matrix_view[0] = Matrix::CreateLookAtLH(position, position + Vector3::Right, Vector3::Up);       // x+
        m_matrix_view[1] = Matrix::CreateLookAtLH(position, position + Vector3::Left, Vector3::Up);       // x-
        m_matrix_view[2] = Matrix::CreateLookAtLH(position, position + Vector3::Up, Vector3::Backward); // y+
        m_matrix_view[3] = Matrix::CreateLookAtLH(position, position + Vector3::Down, Vector3::Forward);  // y-
        m_matrix_view[4] = Matrix::CreateLookAtLH(position, position + Vector3::Forward, Vector3::Up);       // z+
        m_matrix_view[5] = Matrix::CreateLookAtLH(position, position + Vector3::Backward, Vector3::Up);       // z-
    }
}

void Light::ComputeProjectionMatrix(uint32_t index /*= 0*/)
{
    SP_ASSERT(index < m_shadow_map.texture_depth->GetArrayLength());

    ShadowSlice& shadow_slice = m_shadow_map.slices[index];

    if (m_light_type == LightType::Directional)
    {
        const float cascade_depth = (shadow_slice.max.z - shadow_slice.min.z);
        m_matrix_projection[index] = Matrix::CreateOrthoOffCenterLH(shadow_slice.min.x, shadow_slice.max.x, shadow_slice.min.y, shadow_slice.max.y, cascade_depth, 0.0f); // reverse-z
        shadow_slice.frustum = Frustum(m_matrix_view[index], m_matrix_projection[index], cascade_depth);
    }
    else
    {
        const uint32_t width = m_shadow_map.texture_depth->GetWidth();
        const uint32_t height = m_shadow_map.texture_depth->GetHeight();
        const float aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
        const float fov = m_light_type == LightType::Spot ? m_angle_rad * 2.0f : Math::Helper::PI_DIV_2;
        m_matrix_projection[index] = Matrix::CreatePerspectiveFieldOfViewLH(fov, aspect_ratio, m_range, 0.3f); // reverse-z
        shadow_slice.frustum = Frustum(m_matrix_view[index], m_matrix_projection[index], m_range);
    }
}

const Matrix& Light::GetViewMatrix(uint32_t index /*= 0*/) const
{
    SP_ASSERT(index < static_cast<uint32_t>(m_matrix_view.size()));

    return m_matrix_view[index];
}

const Matrix& Light::GetProjectionMatrix(uint32_t index /*= 0*/) const
{
    SP_ASSERT(index < static_cast<uint32_t>(m_matrix_projection.size()));

    return m_matrix_projection[index];
}

void Light::ComputeCascadeSplits()
{
    if (m_shadow_map.slices.empty())
        return;

    // Can happen during the first frame, don't log error
    if (!Renderer::GetMainCamera())
        return;

    RenderCamera* camera = Renderer::GetMainCamera();
    const float clip_near = camera->GetNearPlane();
    const float clip_far = camera->GetFarPlane();
    const Matrix projection = camera->ComputeProjection(clip_near, clip_far); // Non reverse-z matrix
    const Matrix view_projection_inverted = Matrix::Invert(camera->GetViewMatrix() * projection);

    // Calculate split depths based on view camera frustum
    const float split_lambda = 0.98f;
    const float clip_range = clip_far - clip_near;
    const float min_z = clip_near;
    const float max_z = clip_near + clip_range;
    const float range = max_z - min_z;
    const float ratio = max_z / min_z;
    std::vector<float> splits(m_cascade_count);
    for (uint32_t i = 0; i < m_cascade_count; i++)
    {
        const float p = (i + 1) / static_cast<float>(m_cascade_count);
        const float log = min_z * Math::Helper::Pow(ratio, p);
        const float uniform = min_z + range * p;
        const float d = split_lambda * (log - uniform) + uniform;
        splits[i] = (d - clip_near) / clip_range;
    }

    float last_split_distance = 0.0f;
    for (uint32_t i = 0; i < m_cascade_count; i++)
    {
        // Define camera frustum corners in clip space
        Vector3 frustum_corners[8] =
        {
            Vector3(-1.0f,  1.0f, -1.0f),
            Vector3(1.0f,  1.0f, -1.0f),
            Vector3(1.0f, -1.0f, -1.0f),
            Vector3(-1.0f, -1.0f, -1.0f),
            Vector3(-1.0f,  1.0f,  1.0f),
            Vector3(1.0f,  1.0f,  1.0f),
            Vector3(1.0f, -1.0f,  1.0f),
            Vector3(-1.0f, -1.0f,  1.0f)
        };

        // Project frustum corners into world space
        for (Vector3& frustum_corner : frustum_corners)
        {
            Vector4 inverted_corner = Vector4(frustum_corner, 1.0f) * view_projection_inverted;
            frustum_corner = inverted_corner / inverted_corner.w;
        }

        // Compute split distance
        {
            const float split_distance = splits[i];
            for (uint32_t i = 0; i < 4; i++)
            {
                Vector3 distance = frustum_corners[i + 4] - frustum_corners[i];
                frustum_corners[i + 4] = frustum_corners[i] + (distance * split_distance);
                frustum_corners[i] = frustum_corners[i] + (distance * last_split_distance);
            }
            last_split_distance = splits[i];
        }

        // Compute frustum bounds
        {
            // Compute bounding sphere which encloses the frustum.
            // Since a sphere is rotational invariant it will keep the size of the orthographic
            // projection frustum same independent of eye view direction, hence eliminating shimmering.

            ShadowSlice& shadow_slice = m_shadow_map.slices[i];

            // Compute center
            shadow_slice.center = Vector3::Zero;
            for (const Vector3& frustum_corner : frustum_corners)
            {
                shadow_slice.center += Vector3(frustum_corner);
            }
            shadow_slice.center /= 8.0f;

            // Compute radius
            float radius = 0.0f;
            for (const Vector3& frustum_corner : frustum_corners)
            {
                const float distance = Vector3::Distance(frustum_corner, shadow_slice.center);
                radius = Math::Helper::Max(radius, distance);
            }
            radius = Math::Helper::Ceil(radius * 16.0f) / 16.0f;

            // Compute min and max
            shadow_slice.max = radius;
            shadow_slice.min = -radius;
        }
    }
}

uint32_t Light::GetShadowArraySize() const
{
    return m_shadow_map.texture_depth ? m_shadow_map.texture_depth->GetArrayLength() : 0;
}

void Light::CreateShadowMap()
{
    // Early exit if there is no change in shadow map resolution
    const uint32_t resolution = Renderer::GetOption<uint32_t>(Renderer_Option::ShadowResolution);
    const bool resolution_changed = m_shadow_map.texture_depth ? (resolution != m_shadow_map.texture_depth->GetWidth()) : false;
    if ((!m_is_dirty && !resolution_changed))
        return;

    // Early exit if this light casts no shadows
    if (!m_shadows_enabled)
    {
        m_shadow_map.texture_depth.reset();
        return;
    }

    if (!m_shadows_transparent_enabled)
    {
        m_shadow_map.texture_color.reset();
    }

    RHI_Format format_depth = RHI_Format::D32_Float;
    RHI_Format format_color = RHI_Format::R8G8B8A8_Unorm;

    if (GetLightType() == LightType::Directional)
    {
        m_shadow_map.texture_depth = std::make_unique<RHI_Texture2DArray>(resolution, resolution, format_depth, m_cascade_count, RHI_Texture_RenderTarget | RHI_Texture_Srv, "shadow_map_directional");

        if (m_shadows_transparent_enabled)
        {
            m_shadow_map.texture_color = std::make_unique<RHI_Texture2DArray>(resolution, resolution, format_color, m_cascade_count, RHI_Texture_RenderTarget | RHI_Texture_Srv, "shadow_map_directional_color");
        }

        m_shadow_map.slices = std::vector<ShadowSlice>(m_cascade_count);
    }
    else if (GetLightType() == LightType::Point)
    {
        m_shadow_map.texture_depth = std::make_unique<RHI_TextureCube>(resolution, resolution, format_depth, RHI_Texture_RenderTarget | RHI_Texture_Srv, "shadow_map_point_color");

        if (m_shadows_transparent_enabled)
        {
            m_shadow_map.texture_color = std::make_unique<RHI_TextureCube>(resolution, resolution, format_color, RHI_Texture_RenderTarget | RHI_Texture_Srv, "shadow_map_point_color");
        }

        m_shadow_map.slices = std::vector<ShadowSlice>(6);
    }
    else if (GetLightType() == LightType::Spot)
    {
        m_shadow_map.texture_depth = std::make_unique<RHI_Texture2D>(resolution, resolution, 1, format_depth, RHI_Texture_RenderTarget | RHI_Texture_Srv, "shadow_map_spot_color");

        if (m_shadows_transparent_enabled)
        {
            m_shadow_map.texture_color = std::make_unique<RHI_Texture2D>(resolution, resolution, 1, format_color, RHI_Texture_RenderTarget | RHI_Texture_Srv, "shadow_map_spot_color");
        }

        m_shadow_map.slices = std::vector<ShadowSlice>(1);
    }
}

bool Light::IsInViewFrustum(MeshFilter* renderable, uint32_t index) const
{
    const auto box = renderable->GetAabb();
    const auto center = box.GetCenter();
    const auto extents = box.GetExtents();

    // ensure that potential shadow casters from behind the near plane are not rejected
    const bool ignore_near_plane = (m_light_type == LightType::Directional) ? true : false;

    return m_shadow_map.slices[index].frustum.IsVisible(center, extents, ignore_near_plane);
}
