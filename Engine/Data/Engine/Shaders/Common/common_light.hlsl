  
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
    float3 lightDir = light_buffer_data.forward.xyz;
    float3 lightColor = light_buffer_data.color.rgb;
    float luminosity = light_buffer_data.intensity;


    return BilinnPhong(viewDir, normal, diffuseTex, specularTex, shininess, lightDir, lightColor, luminosity);
}


float3 CalcDirectionalLight(float3 viewDir, float3 normal, float3 diffuseTex, float3 specularTex, float shininess, LightBufferData light_buffer_data, float luminosity)
{
    return BilinnPhong(viewDir, normal, diffuseTex, specularTex, shininess, light_buffer_data.forward.xyz, light_buffer_data.color.rgb, luminosity);
}


// return shadow ratio
float ShadowCalculation(float3 fragWorldNormal, float3 fragWorldPos)
{
    // default shadow value for fully lit (no shadow)
    float shadow = 1.0f;
    int mainLightIndex = 0;
    if (buffer_lights[mainLightIndex].light_has_shadows())
    {
        LightBufferData light = buffer_lights[mainLightIndex];
        
        // process only if the pixel is within the light's effective range
        float distance_to_pixel = length(fragWorldPos - light.position);
        if (distance_to_pixel < light.range)
        {
            
            // project to light space
            // uint slice_index = light.light_is_point() ? direction_to_cube_face_index(light.to_pixel) : 0;
            uint slice_index = 0;
        
            float3 light_to_pixel = light.compute_direction(fragWorldPos);
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
        // process only if the pixel is within the light's effective range
        float distance_to_pixel = length(fragWorldPos - light.position);
        if (distance_to_pixel < light.range)
        {
            float3 light_to_pixel = light.compute_direction(fragWorldPos);
            float light_n_dot_l = saturate(dot(fragWorldNormal, -light_to_pixel));
        
            float2 resolution = light.compute_resolution();
            float2 texel_size = 1.0f / resolution;
            float3 normal_offset_bias = fragWorldNormal * (1.0f - saturate(light_n_dot_l)) * texel_size.x;
            float3 position_world = fragWorldPos + normal_offset_bias;
        

            if (light.light_is_point())
            {
             // compute paraboloid coordinates and depth
                uint light_slice_index = dot(light.forward, light_to_pixel) < 0.0f; // 0 = front, 1 = back
                uint slice_index = 2 * lightIndex + light_slice_index;
                float3 pos_view = mul(float4(position_world, 1.0f), light.view_projection[light_slice_index]).xyz;
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
                uint light_slice_index = 2 * lightIndex + 0;
                uint slice_index = 2 * lightIndex + light_slice_index;

                // project into light space
                float3 pos_ndc = world_to_ndc(position_world, light.view_projection[light_slice_index]);
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
                        light_slice_index = 2 * lightIndex + 1;
                        slice_index = 2 * lightIndex + light_slice_index;
                        pos_ndc = world_to_ndc(position_world, light.view_projection[light_slice_index]);
                        pos_uv = ndc_to_uv(pos_ndc);
                        float shadow_far = SampleShadowMap(light, float3(pos_uv, slice_index), pos_ndc.z);

                        // blend/lerp
                        shadow = lerp(shadow, shadow_far, cascade_fade);
                    }
                }
            }

        }
       
    }

 
    return shadow;
}



float3 CalcOneLightColorPBR(
float3 fragPos,
float4 albedo, float metallic, float squareRoughness, float lerpSquareRoughness,
float3 normal, float3 viewDir, float nv,
int light_index)
{
    float PI = 3.14;
    float3 colorSpaceDielectricSpecRgb = float3(0.04, 0.04, 0.04);

    LightBufferData lightBufferData = buffer_lights[light_index];
    float3 light_to_pixel = lightBufferData.compute_direction(fragPos);
    float3 lightDir = -light_to_pixel;
    float attenuation = lightBufferData.compute_attenuation(fragPos);
    float3 halfVector = normalize(lightDir + viewDir);

    float nl = max(saturate(dot(normal, lightDir)), 0.000001);
    float lh = max(saturate(dot(lightDir, halfVector)), 0.000001);
    float vh = max(saturate(dot(viewDir, halfVector)), 0.000001);
    float nh = max(saturate(dot(normal, halfVector)), 0.000001);

    float3 randiance = lightBufferData.color.xyz * lightBufferData.intensity * attenuation * nl * 1.0f;;
    
	// temp code
    float shadow = 0;
    bool t = pass_is_transparent();
    if (t)
    {
        shadow = ShadowCalculation2(normal, fragPos, light_index);
    }
    else
    {
        shadow = ShadowCalculation2(normal, fragPos, light_index);
    }

    randiance *= shadow;

	// calculate D F G for Specular
    float D = lerpSquareRoughness / (pow((pow(nh, 2) * (lerpSquareRoughness - 1) + 1), 2) * PI);

    float3 F0 = lerp(colorSpaceDielectricSpecRgb, albedo.xyz, metallic);
    float3 F = F0 + (1 - F0) * exp2((-5.55473 * vh - 6.98316) * vh);

    float kInDirectLight = pow(squareRoughness + 1, 2) / 8;
    float kInIBL = pow(squareRoughness, 2) / 8;
    float GLeft = nl / lerp(nl, 1, kInDirectLight);
    float GRight = nv / lerp(nv, 1, kInDirectLight);
    float G = GLeft * GRight;

    float3 kd = (1 - F) * (1 - metallic);

	// calculate directLightResult with diffuse and specular
    float3 diffColor = kd * albedo.xyz / PI;
    float3 specColor = (D * G * F) / (4 * nv * nl);
    float3 directLightResult = (diffColor + specColor) * randiance;

	// calculate indirectLightResult todo
    float3 iblDiffuseResult = 0;
    float3 iblSpecularResult = 0;
    float3 indirectResult = iblDiffuseResult + iblSpecularResult;

    return directLightResult + indirectResult;
}
