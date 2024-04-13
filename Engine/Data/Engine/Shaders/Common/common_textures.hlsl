
// // G-buffer
Texture2D tex_depth             : register(t16);

 // Light depth/color maps
Texture2DArray tex_light_directional_depth : register(t20);
Texture2DArray tex_light_directional_color : register(t21);
TextureCube tex_light_point_depth : register(t22);
TextureCube tex_light_point_color : register(t23);
Texture2D tex_light_spot_depth : register(t24);
Texture2D tex_light_spot_color : register(t25);

// // Misc
Texture2D tex_frame              : register(t34);
Texture2D tex                    : register(t35);
Texture2D tex_font_atlas         : register(t37);

// // Storage
// RWTexture2D<float4> tex_uav                                : register(u0);
// RWTexture2D<float4> tex_uav2                               : register(u1);
// RWTexture2D<float4> tex_uav3                               : register(u2);
// globallycoherent RWStructuredBuffer<uint> g_atomic_counter : register(u3);
// globallycoherent RWTexture2D<float4> tex_uav_mips[12]      : register(u4);
// RWTexture2DArray<float4> tex_uav4                          : register(u5);

// SkyBox
TextureCube tex_skyBox : register(t40);