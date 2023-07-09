#shader vertex
#version 430 core

layout (location = 0) in vec3 geo_Pos;
layout (location = 1) in vec2 geo_TexCoords;
layout (location = 2) in vec3 geo_Normal;
layout (location = 3) in vec3 geo_Tangent;
layout (location = 4) in vec3 geo_Bitangent;
layout (location = 5) in ivec4 geo_boneIdArr;
layout (location = 6) in vec3 geo_boneWeightArr;

/* Global information sent by the engine */
layout (std140) uniform EngineUBO
{
    mat4    ubo_Model;
    mat4    ubo_View;
    mat4    ubo_Projection;
    vec3    ubo_ViewPos;
    float   ubo_Time;
};

/* Information passed to the fragment shader */
out VS_OUT
{
    vec3        FragPos;
    vec3        Normal;
    vec2        TexCoords;
    mat3        TBN;
    flat vec3   TangentViewPos;
    vec3        TangentFragPos;
    vec4 ShadowCoord;
} vs_out;

uniform mat4 ubo_LightSpaceMatrix;

const int MAX_BONES = 250;

uniform mat4 ubo_boneFinalMatrixArr[MAX_BONES];

void main()
{

    /*Bone Animation*/
	vec4 weights = vec4(geo_boneWeightArr.x, geo_boneWeightArr.y, geo_boneWeightArr.z, 1.0f - geo_boneWeightArr.x - geo_boneWeightArr.y - geo_boneWeightArr.z);
    mat4 boneTransform = ubo_boneFinalMatrixArr[geo_boneIdArr[0]] * weights[0];
    boneTransform += ubo_boneFinalMatrixArr[geo_boneIdArr[1]] * weights[1];
    boneTransform += ubo_boneFinalMatrixArr[geo_boneIdArr[2]] * weights[2];
    boneTransform += ubo_boneFinalMatrixArr[geo_boneIdArr[3]] * weights[3];

    vec4 PosL = boneTransform * vec4(geo_Pos, 1.0);
    vec4 NormalL = boneTransform * vec4(geo_Normal, 0.0);
    vec4 TangentL = boneTransform * vec4(geo_Tangent, 0.0);
    vec4 BitangentL = boneTransform * vec4(geo_Bitangent, 0.0);

    vs_out.TBN = mat3
    (
        normalize(vec3(ubo_Model * TangentL)),
        normalize(vec3(ubo_Model * BitangentL)),
        normalize(vec3(ubo_Model * NormalL))
    );

    mat3 TBNi = transpose(vs_out.TBN);

    vs_out.FragPos          = vec3(ubo_Model * PosL);
    vs_out.Normal           = vec3(normalize(ubo_Model * NormalL));
    vs_out.TexCoords        = geo_TexCoords;
    vs_out.TangentViewPos   = TBNi * ubo_ViewPos;
    vs_out.TangentFragPos   = TBNi * vs_out.FragPos;

    gl_Position = ubo_Projection * ubo_View * vec4(vs_out.FragPos, 1.0);

    vs_out.ShadowCoord = ubo_LightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
}

#shader fragment
#version 430 core

/* Global information sent by the engine */
layout (std140) uniform EngineUBO
{
    mat4    ubo_Model;
    mat4    ubo_View;
    mat4    ubo_Projection;
    vec3    ubo_ViewPos;
    float   ubo_Time;
};

/* Information passed from the fragment shader */
in VS_OUT
{
    vec3        FragPos;
    vec3        Normal;
    vec2        TexCoords;
    mat3        TBN;
    flat vec3   TangentViewPos;
    vec3        TangentFragPos;
    vec4 ShadowCoord;
} fs_in;

/* Light information sent by the engine */
layout(std430, binding = 0) buffer LightSSBO
{
    mat4 ssbo_Lights[];
};

/* Uniforms (Tweakable from the material editor) */
uniform vec2        u_TextureTiling           = vec2(1.0, 1.0);
uniform vec2        u_TextureOffset           = vec2(0.0, 0.0);
uniform vec4        u_Diffuse                 = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec3        u_Specular                = vec3(1.0, 1.0, 1.0);
uniform float       u_Shininess               = 100.0;
uniform float       u_HeightScale             = 0.0;
uniform bool        u_EnableNormalMapping     = false;
uniform sampler2D   u_DiffuseMap;
uniform sampler2D   u_SpecularMap;
uniform sampler2D   u_NormalMap;
uniform sampler2D   u_HeightMap;
uniform sampler2D   u_MaskMap;

uniform sampler2D u_shadowMap;

/* Global variables */
vec3 g_Normal;
vec2 g_TexCoords;
vec3 g_ViewDir;
vec4 g_DiffuseTexel;
vec4 g_SpecularTexel;
vec4 g_HeightTexel;
vec4 g_NormalTexel;

out vec4 FRAGMENT_COLOR;

vec3 UnPack(float p_Target)
{
    return vec3
    (
        float((uint(p_Target) >> 24) & 0xff)    * 0.003921568627451,
        float((uint(p_Target) >> 16) & 0xff)    * 0.003921568627451,
        float((uint(p_Target) >> 8) & 0xff)     * 0.003921568627451
    );
}

bool PointInAABB(vec3 p_Point, vec3 p_AabbCenter, vec3 p_AabbHalfSize)
{
    return
    (
        p_Point.x > p_AabbCenter.x - p_AabbHalfSize.x && p_Point.x < p_AabbCenter.x + p_AabbHalfSize.x &&
        p_Point.y > p_AabbCenter.y - p_AabbHalfSize.y && p_Point.y < p_AabbCenter.y + p_AabbHalfSize.y &&
        p_Point.z > p_AabbCenter.z - p_AabbHalfSize.z && p_Point.z < p_AabbCenter.z + p_AabbHalfSize.z
    );
}

vec2 ParallaxMapping(vec3 p_ViewDir)
{
    const vec2 parallax = p_ViewDir.xy * u_HeightScale * texture(u_HeightMap, g_TexCoords).r;
    return g_TexCoords - vec2(parallax.x, 1.0 - parallax.y);
}

vec3 BlinnPhong(vec3 p_LightDir, vec3 p_LightColor, float p_Luminosity)
{
    const vec3  halfwayDir          = normalize(p_LightDir + g_ViewDir);
    const float diffuseCoefficient  = max(dot(g_Normal, p_LightDir), 0.0);
    const float specularCoefficient = pow(max(dot(g_Normal, halfwayDir), 0.0), u_Shininess * 2.0);

    return p_LightColor * g_DiffuseTexel.rgb * diffuseCoefficient * p_Luminosity + ((p_Luminosity > 0.0) ? (p_LightColor * g_SpecularTexel.rgb * specularCoefficient * p_Luminosity) : vec3(0.0));
}

float LuminosityFromAttenuation(mat4 p_Light)
{
    const vec3  lightPosition   = p_Light[0].rgb;
    const float constant        = p_Light[0][3];
    const float linear          = p_Light[1][3];
    const float quadratic       = p_Light[2][3];

    const float distanceToLight = length(lightPosition - fs_in.FragPos);
    const float attenuation     = (constant + linear * distanceToLight + quadratic * (distanceToLight * distanceToLight));
    return 1.0 / attenuation;
}

vec3 CalcPointLight(mat4 p_Light)
{
    /* Extract light information from light mat4 */
    const vec3 lightPosition  = p_Light[0].rgb;
    const vec3 lightColor     = UnPack(p_Light[2][0]);
    const float intensity     = p_Light[3][3];

    const vec3  lightDirection  = normalize(lightPosition - fs_in.FragPos);
    const float luminosity      = LuminosityFromAttenuation(p_Light);

    return BlinnPhong(lightDirection, lightColor, intensity * luminosity);
}

vec3 CalcDirectionalLight(mat4 light)
{
    return BlinnPhong(-light[1].rgb, UnPack(light[2][0]), light[3][3]);
}

vec3 CalcSpotLight(mat4 p_Light)
{
    /* Extract light information from light mat4 */
    const vec3  lightPosition   = p_Light[0].rgb;
    const vec3  lightForward    = p_Light[1].rgb;
    const vec3  lightColor      = UnPack(p_Light[2][0]);
    const float intensity       = p_Light[3][3];
    const float cutOff          = cos(radians(p_Light[3][1]));
    const float outerCutOff     = cos(radians(p_Light[3][1] + p_Light[3][2]));

    const vec3  lightDirection  = normalize(lightPosition - fs_in.FragPos);
    const float luminosity      = LuminosityFromAttenuation(p_Light);

    /* Calculate the spot intensity */
    const float theta           = dot(lightDirection, normalize(-lightForward)); 
    const float epsilon         = cutOff - outerCutOff;
    const float spotIntensity   = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);
    
    return BlinnPhong(lightDirection, lightColor, intensity * spotIntensity * luminosity);
}

vec3 CalcAmbientBoxLight(mat4 p_Light)
{
    const vec3  lightPosition   = p_Light[0].rgb;
    const vec3  lightColor      = UnPack(p_Light[2][0]);
    const float intensity       = p_Light[3][3];
    const vec3  size            = vec3(p_Light[0][3], p_Light[1][3], p_Light[2][3]);

    return PointInAABB(fs_in.FragPos, lightPosition, size) ? g_DiffuseTexel.rgb * lightColor * intensity : vec3(0.0);
}

vec3 CalcAmbientSphereLight(mat4 p_Light)
{
    const vec3  lightPosition   = p_Light[0].rgb;
    const vec3  lightColor      = UnPack(p_Light[2][0]);
    const float intensity       = p_Light[3][3];
    const float radius          = p_Light[0][3];

    return distance(lightPosition, fs_in.FragPos) <= radius ? g_DiffuseTexel.rgb * lightColor * intensity : vec3(0.0);
}

// 返回有多少处于阴影的量
float ShadowCalculation(vec4 fragPosLightSpace)
{
    // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = texture(u_shadowMap, projCoords.xy).r; 
    // 取得当前片段在光源视角下的深度
    float currentDepth = projCoords.z;
    // 检查当前片段是否在阴影中
    float bias = 0.05;
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

void main()
{
    g_TexCoords = u_TextureOffset + vec2(mod(fs_in.TexCoords.x * u_TextureTiling.x, 1), mod(fs_in.TexCoords.y * u_TextureTiling.y, 1));

    /* Apply parallax mapping */
    if (u_HeightScale > 0)
        g_TexCoords = ParallaxMapping(normalize(fs_in.TangentViewPos - fs_in.TangentFragPos));
        
    
	float shadow = ShadowCalculation(fs_in.ShadowCoord);

    /* Apply color mask */
    if (texture(u_MaskMap, g_TexCoords).r != 0.0)
    {
        g_ViewDir           = normalize(ubo_ViewPos - fs_in.FragPos);
        g_DiffuseTexel      = texture(u_DiffuseMap,  g_TexCoords) * u_Diffuse;
        g_SpecularTexel     = texture(u_SpecularMap, g_TexCoords) * vec4(u_Specular, 1.0);

        if (u_EnableNormalMapping)
        {
            g_Normal = texture(u_NormalMap, g_TexCoords).rgb;
            g_Normal = normalize(g_Normal * 2.0 - 1.0);   
            g_Normal = normalize(fs_in.TBN * g_Normal);
        }
        else
        {
            g_Normal = normalize(fs_in.Normal);
        }

        vec3 lightSum = vec3(0.0);

        for (int i = 0; i < ssbo_Lights.length(); ++i)
        {
            switch(int(ssbo_Lights[i][3][0]))
            {
                case 0: lightSum += CalcPointLight(ssbo_Lights[i]);         break;
                case 1: lightSum += CalcDirectionalLight(ssbo_Lights[i]);   break;
                case 2: lightSum += CalcSpotLight(ssbo_Lights[i]);          break;
                case 3: lightSum += CalcAmbientBoxLight(ssbo_Lights[i]);    break;
                case 4: lightSum += CalcAmbientSphereLight(ssbo_Lights[i]); break;
            }
        }
		
		// Ambient
		vec3 shadowedColor = (1.0f - shadow) * lightSum;
        FRAGMENT_COLOR = vec4(shadowedColor, g_DiffuseTexel.a);
    }
    else
    {
        FRAGMENT_COLOR = vec4(0.0);
    }
}