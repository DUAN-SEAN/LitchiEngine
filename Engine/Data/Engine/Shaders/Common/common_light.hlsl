
#include "../Common/common.hlsl"
#include "../Common/shadow_mapping.hlsl"

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


// return shadow ratio
float ShadowCalculation(float3 fragWorldPos)
{
    // todo only use index 0
	// project into light space
    float shadow = 0.0f;
    int shadowIndex = 0;
    LightBufferData light = light_buffer_data_arr.lightBufferDataArr[shadowIndex];
    // 检查是否在视口范围内

    // project to light space
    // uint slice_index = light.light_is_point() ? direction_to_cube_face_index(light.to_pixel) : 0;
    uint slice_index = 0;
    float3 pos_ndc = world_to_ndc(fragWorldPos, light.view_projection[slice_index]);
    float2 pos_uv = ndc_to_uv(pos_ndc);

    if (is_valid_uv(pos_uv))
    {
        // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
        shadow = SampleShadowMap(light, float3(pos_uv, slice_index), pos_ndc.z);
    }

    return shadow;
}
