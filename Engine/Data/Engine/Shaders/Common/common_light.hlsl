  
#include "common.hlsl"
#include "shadow_mapping.hlsl"

/**
 * \brief Light Render Model For BilinnPhong
 * \param viewDir 
 * \param normal 
 * \param diffuseTex 
 * \param specularTex 
 * \param shininess 
 * \param lightDir 
 * \param lightColor 
 * \param luminosity 
 * \return 
 */
float3 BilinnPhong(float3 viewDir, float3 normal, float3 diffuseTex, float3 specularTex, float shininess, float3 lightDir, float3 lightColor, float luminosity)
{
    const float3 halfwayDir = normalize(lightColor + viewDir);
    const float diffuseCoefficient = max(dot(normal, lightDir), 0.0);
    const float specularCoefficient = pow(max(dot(normal, halfwayDir), 0.0), shininess * 2.0);

    return lightColor * diffuseTex.rgb * diffuseCoefficient * luminosity + ((luminosity > 0.0) ?
    (lightColor * specularTex.rgb * specularCoefficient * luminosity) : float3(0, 0, 0));
}

float3 CalcPointLight(float3 viewDir, float3 normal, float3 diffuseTex, float3 specularTex, float shininess, LightBufferData light_buffer_data)
{
    float3 lightPosition = light_buffer_data.position.xyz;
    float3 lightDir = light_buffer_data.direction.xyz;
    float3 lightColor = light_buffer_data.color.rgb;
    float luminosity = light_buffer_data.intensity;


    return BilinnPhong(viewDir, normal, diffuseTex, specularTex, shininess, lightDir, lightColor, luminosity);
}


float3 CalcDirectionalLight(float3 viewDir, float3 normal, float3 diffuseTex, float3 specularTex, float shininess, LightBufferData light_buffer_data, float luminosity)
{
    return BilinnPhong(viewDir, normal, diffuseTex, specularTex, shininess, light_buffer_data.direction.xyz, light_buffer_data.color.rgb, luminosity);
}

float3 compute_direction(LightBufferData light, float3 fragment_position)
{
    float3 direction = 0.0f;
        
    if (light.light_is_directional())
    {
        direction = normalize(light.direction.xyz);
    }
    else if (light.light_is_point() || light.light_is_spot())
    {
        direction = normalize(fragment_position - light.position.xyz);
    }

    return direction;
}

// attenuation over distance
float compute_attenuation_distance(const float3 surface_position, const float3 position,const float far)
{
    float distance_to_pixel = length(surface_position - position);
    float attenuation = saturate(1.0f - distance_to_pixel / far);
    return attenuation * attenuation;
}

float compute_attenuation_angle(float angle,float3 to_pixel,float3 forward)
{
    float cos_outer = cos(angle);
    float cos_inner = cos(angle * 0.9f);
    float cos_outer_squared = cos_outer * cos_outer;
    float scale = 1.0f / max(0.001f, cos_inner - cos_outer);
    float offset = -cos_outer * scale;
    float cd = dot(to_pixel, forward);
    float attenuation = saturate(cd * scale + offset);
        
    return attenuation * attenuation;
}

float compute_attenuation(LightBufferData light,const float3 surface_position)
{
    float attenuation = 0.0f;
        
    if (light.light_is_directional())
    {
        attenuation = saturate(dot(-light.direction, float3(0.0f, 1.0f, 0.0f)));
    }
    else if (light.light_is_point())
    {
        attenuation = compute_attenuation_distance(surface_position,light.position,light.range);
    }
    else if (light.light_is_spot())
    {
        attenuation = compute_attenuation_distance(surface_position, light.position, light.range) * 
        compute_attenuation_angle(light.angle,compute_direction(light,surface_position),light.direction);
    }

    return attenuation;
}

// return shadow ratio
float ShadowCalculation(float3 fragWorldNormal, float3 fragWorldPos)
{
    float shadow = 1.0f;
    int mainLightIndex = 0;
    if (buffer_lights[mainLightIndex].light_has_shadows())
    {
        int shadowIndex = 0;
        LightBufferData light = buffer_lights[mainLightIndex];
        // 检查是否在视口范围内

        // project to light space
        // uint slice_index = light.light_is_point() ? direction_to_cube_face_index(light.to_pixel) : 0;
        uint slice_index = 0;
        
        float3 light_to_pixel = compute_direction(light, fragWorldPos);
        float light_n_dot_l = saturate(dot(fragWorldNormal, -light_to_pixel));
        
        float2 resolution = light.compute_resolution();
        float2 texel_size = 1.0f / resolution;

        // compute world position with normal offset bias to reduce shadow acne
        //float3 normal_offset_bias = surface.normal * (1.0f - saturate(light.n_dot_l)) * light.normal_bias * get_shadow_texel_size();
        //float3 normal_offset_bias = fragWorldNormal * (1.0f - saturate(light_n_dot_l)) * light.normal_bias * 1.0f;
        float3 normal_offset_bias = fragWorldNormal * (1.0f - saturate(light_n_dot_l)) * texel_size.x * 200.0f;
        // float3 normal_offset_bias = fragWorldNormal * light.normal_bias;
        float3 position_world = fragWorldPos + normal_offset_bias;
        
	    // project into light space
        float3 pos_ndc = world_to_ndc(position_world, light.view_projection[slice_index]);
        float2 pos_uv = ndc_to_uv(pos_ndc);

        if (is_valid_uv(pos_uv))
        {
        // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
            shadow = SampleShadowMap(light, float3(pos_uv, slice_index), pos_ndc.z);
        }

        // blend with the far cascade for the directional lights
        float cascade_fade = saturate((max(abs(pos_ndc.x), abs(pos_ndc.y)) - g_shadow_cascade_blend_threshold) * 4.0f);
        if (light.light_is_directional() && cascade_fade > 0.0f)
        {
            // sample shadow map
            slice_index = 1;
            pos_ndc = world_to_ndc(position_world, light.view_projection[slice_index]);
            pos_uv = ndc_to_uv(pos_ndc);
            float shadow_far = SampleShadowMap(light, float3(pos_uv, slice_index), pos_ndc.z);

            // blend/lerp
            shadow = lerp(shadow, shadow_far, cascade_fade);
        }
    }

 
    return shadow;
}


// return shadow ratio
float ShadowCalculation2(float3 fragWorldNormal, float3 fragWorldPos, int lightIndex)
{
    float shadow = 1.0f;
    LightBufferData light = buffer_lights[lightIndex];
    if (light.light_has_shadows())
    {
        float3 light_to_pixel = compute_direction(light, fragWorldPos);
        float light_n_dot_l = saturate(dot(fragWorldNormal, -light_to_pixel));
        
        float2 resolution = light.compute_resolution();
        float2 texel_size = 1.0f / resolution;
        float3 normal_offset_bias = fragWorldNormal * (1.0f - saturate(light_n_dot_l)) * texel_size.x;
        float3 position_world = fragWorldPos + normal_offset_bias;
        

        if (light.light_is_point())
        {
             // compute paraboloid coordinates and depth
            uint slice_index = 2 * lightIndex + dot(light.direction, light_to_pixel) < 0.0f; // 0 = front, 1 = back
            float3 pos_view = mul(float4(position_world, 1.0f), light.view_projection[slice_index]).xyz;
            float3 light_to_vertex_view = pos_view;
            float3 ndc = project_onto_paraboloid(light_to_vertex_view, 0.01, light.range);

            // sample shadow map
            float3 sample_coords = float3(ndc_to_uv(ndc.xy), slice_index);
            shadow = SampleShadowMap(light, sample_coords, ndc.z);

            // // handle transparent shadows if necessary
            //if (shadow.a > 0.0f && light.has_shadows_transparent())
            //{
            //    shadow.rgb = Technique_Vogel_Color(light, surface, sample_coords);
            //}
        }
        else
        {
            // project to light space
            uint slice_index = 2 * lightIndex + 0;

            // project into light space
            float3 pos_ndc = world_to_ndc(position_world, light.view_projection[slice_index]);
            float2 pos_uv = ndc_to_uv(pos_ndc);

            if (is_valid_uv(pos_uv))
            {
                shadow = SampleShadowMap(light, float3(pos_uv, slice_index), pos_ndc.z);

                //if (shadow.a > 0.0f && light.has_shadows_transparent())
                //{
                //    shadow.rgb = Technique_Vogel_Color(light, surface, sample_coords);
                //}

                  // blend with the far cascade for the directional lights
                float cascade_fade = saturate((max(abs(pos_ndc.x), abs(pos_ndc.y)) - g_shadow_cascade_blend_threshold) * 4.0f);
                if (light.light_is_directional() && cascade_fade > 0.0f)
                {
                    // sample shadow map
                    slice_index = 2 * lightIndex +1;
                    pos_ndc = world_to_ndc(position_world, light.view_projection[slice_index]);
                    pos_uv = ndc_to_uv(pos_ndc);
                    float shadow_far = SampleShadowMap(light, float3(pos_uv, slice_index), pos_ndc.z);

                    // blend/lerp
                    shadow = lerp(shadow, shadow_far, cascade_fade);
                }
            }
        }

    }

 
    return shadow;
}

