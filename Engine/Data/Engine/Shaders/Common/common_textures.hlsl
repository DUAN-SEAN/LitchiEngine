
// // G-buffer
// Texture2D tex_albedo            : register(t10);
// Texture2D tex_normal            : register(t11);
// Texture2D tex_material          : register(t12);
// Texture2D tex_material_2        : register(t13);
// Texture2D tex_velocity          : register(t14);
// Texture2D tex_velocity_previous : register(t15);
Texture2D tex_depth             : register(t16);

// Texture2D tex_light_diffuse              : register(t17);
// Texture2D tex_light_diffuse_transparent  : register(t18);
// Texture2D tex_light_specular             : register(t19);
// Texture2D tex_light_specular_transparent : register(t20);
// Texture2D tex_light_volumetric           : register(t21);

 // Light depth/color maps
Texture2DArray tex_light_directional_depth : register(t20);
Texture2DArray tex_light_directional_color : register(t21);
TextureCube tex_light_point_depth : register(t22);
TextureCube tex_light_point_color : register(t23);
Texture2D tex_light_spot_depth : register(t24);
Texture2D tex_light_spot_color : register(t25);

// // Noise
// Texture2D tex_noise_normal    : register(t28);
// Texture2DArray tex_noise_blue : register(t29);

// // Misc
// Texture2D tex_lut_ibl            : register(t30);
Texture2D tex_environment        : register(t31);
Texture2D tex_ssgi               : register(t32);
Texture2D tex_ssr                : register(t33);
Texture2D tex_frame              : register(t34);
Texture2D tex                    : register(t35);
Texture2D tex2                   : register(t36);
Texture2D tex_font_atlas         : register(t37);
// TextureCube tex_reflection_probe : register(t38);
// Texture2DArray tex_sss			 : register(t39);

// // Storage
// RWTexture2D<float4> tex_uav                                : register(u0);
// RWTexture2D<float4> tex_uav2                               : register(u1);
// RWTexture2D<float4> tex_uav3                               : register(u2);
// globallycoherent RWStructuredBuffer<uint> g_atomic_counter : register(u3);
// globallycoherent RWTexture2D<float4> tex_uav_mips[12]      : register(u4);
// RWTexture2DArray<float4> tex_uav4                          : register(u5);

// SkyBox
TextureCube tex_skyBox : register(t40);