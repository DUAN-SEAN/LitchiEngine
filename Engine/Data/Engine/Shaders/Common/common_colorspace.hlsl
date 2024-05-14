
// for details, read my blog post: https://panoskarabelas.com/blog/posts/hdr_in_under_10_minutes/

static const float gamma_sdr = 2.2f; // SDR monitors are likely old, and aim for a simplistic gamma 2.2 curve
static const float gamma_hdr = 2.4f; // HDR monitors are more likely to aim for the actual sRGB standard, which has a curve that for mid tones to high lights resembles a gamma of 2.4

float3 srgb_to_linear(float3 color)
{
    float gamma        = lerp(gamma_sdr, gamma_hdr, false);
    float3 linear_low  = color / 12.92;
    float3 linear_high = pow((color + 0.055) / 1.055, gamma);
    float3 is_high     = step(0.0404482362771082, color);
    return lerp(linear_low, linear_high, is_high);
}

float3 linear_to_srgb(float3 color)
{
    float gamma = lerp(gamma_sdr, gamma_hdr, false);
    float3 srgb_low  = color * 12.92;
    float3 srgb_high = 1.055 * pow(color, 1.0 / gamma) - 0.055;
    float3 is_high   = step(0.00313066844250063, color);
    return lerp(srgb_low, srgb_high, is_high);
}

float3 linear_to_hdr10(float3 color, float white_point)
{
    // convert Rec.709 (similar to srgb) to Rec.2020 color space
    {
        static const float3x3 from709to2020 =
        {
            { 0.6274040f, 0.3292820f, 0.0433136f },
            { 0.0690970f, 0.9195400f, 0.0113612f },
            { 0.0163916f, 0.0880132f, 0.8955950f }
        };
        
        color = mul(from709to2020, color);
    }

    // normalize HDR scene values ([0..>1] to [0..1]) for the ST.2084 curve
    const float st2084_max = 10000.0f;
    color *= white_point / st2084_max;

    // apply ST.2084 (PQ curve) for HDR10 standard
    {
        static const float m1 = 2610.0 / 4096.0 / 4;
        static const float m2 = 2523.0 / 4096.0 * 128;
        static const float c1 = 3424.0 / 4096.0;
        static const float c2 = 2413.0 / 4096.0 * 32;
        static const float c3 = 2392.0 / 4096.0 * 32;
        float3 cp             = pow(abs(color), m1);
        color                 = pow((c1 + c2 * cp) / (1 + c3 * cp), m2);
    }

    return color;
}
